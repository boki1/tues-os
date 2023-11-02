#include <iostream>
#include <new>
#include <algorithm>

#include <sys/mman.h>

#include "OAllocator.h"

uint64_t ceil_p2(uint64_t x, uint64_t p2);

// --------------- SIZE_ADDR ---------------

bool operator<(const size_addr &l, const size_addr &r)
{
    return l.size == r.size ? l.addr < r.addr : l.size < r.size;
}

// --------------- OALLOCATOR ---------------

OAllocator::OAllocator()
{
    if(!size2addr.empty()) return;

    size_addr first_block = {
            .size =  MAX_SIZE >> 2, // 16 TB
            .addr =  mmap(nullptr, MAX_SIZE >> 2, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
    };

    if (!first_block.addr)
        throw std::bad_alloc();

    size2addr[first_block] = nullptr;
}

OAllocator& OAllocator::get_allocator()
{
    static OAllocator allocator;
    return allocator;
}

void* OAllocator::xalloc(size_t size)
{
    size = ceil_p2(size, PAGE_SIZE);

//    if(size < ENTRY_SIZE - HEADER_SIZE)
//    {
//        void *ptr = heap.alloc(&heap, size);
//        addr2size[ptr] = size;
//        return ptr;
//    }

    // find free space
    auto key_iter = std::lower_bound(size2addr.cbegin(), size2addr.cend(), size, [](const auto& curr, size_t size) {
        return curr.first.size < size;
    });

    // empty iterator
    if(size2addr.cend() == key_iter)
        return nullptr;

    size_addr key = key_iter->first;

    // pop the key
    size2addr.erase(key);

    // create new key
    size_addr new_key = {
            .size = key.size - size,
            .addr = (char*)key.addr + size,
    };

    // insert the new key
    if(!new_key.size)
        size2addr[new_key] = nullptr;

    // add the allocated address to the map with allocated addresses
    addr2size[key.addr] = size;

    if(-1 == mprotect(key.addr, size, PROT_WRITE))
        throw std::exception();

    return key.addr;
}

void OAllocator::xfree(void* ptr)
{
    if(!ptr)
        throw std::exception();

    size_t s = addr2size[ptr];

    size_addr pointer = {
            .size = s,
            .addr = ptr,
    };

    // pop the ptr from the map with allocated addresses
    addr2size.erase(ptr);

//    if(pointer.size < ENTRY_SIZE - HEADER_SIZE)
//    {
//        heap.free(&heap, pointer.addr, pointer.size);
//        return;
//    }

    if(-1 == mprotect(pointer.addr, pointer.size, PROT_NONE))
        throw std::exception();

    // iterate through the map and coalesce if able to
    size_addr left_chunk{}, right_chunk{};

    for(auto pair : size2addr)
    {
        size_addr pair_key = pair.first;

        if((char*)pointer.addr + pointer.size == pair_key.addr)
            right_chunk = pair_key;
        else if((char*)pair_key.addr + pair_key.size == pointer.addr)
            left_chunk = pair_key;

        if(left_chunk.size && right_chunk.size) break;
    }

    size_addr coalesced_key = {
            .size = pointer.size + left_chunk.size + right_chunk.size,
            .addr = pointer.addr,
    };

    if(left_chunk.size)
    {
        coalesced_key.addr = left_chunk.addr;
        size2addr.erase(left_chunk);
    }
    if(right_chunk.size)
        size2addr.erase(right_chunk);

    size2addr[coalesced_key] = nullptr;
}