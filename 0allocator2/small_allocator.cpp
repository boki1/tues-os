#include <sys/mman.h>
#include <cstdio>

#include "small_allocator.h"

#ifndef ASSERT_ERROR
    #define ASSERT_ERROR(expr, msg) if(!(expr)) { fprintf(stderr, msg); return -1;}
    #define ASSERT_ERROR_NULL(expr, msg) if(!(expr)) { fprintf(stderr, msg); return NULL;}
#endif

#define NULL nullptr

// ------------------ PRIVATE FUNCTION DECLARATIONS ------------------

uint64_t ceil_p2(uint64_t x, uint64_t p2);

uint64_t floor_p2(uint64_t x, uint64_t p2);

int entry_header_commit(void *entry, uint32_t bit_index, uint32_t bit_count);

int entry_header_decommit(void *entry, uint32_t bit_index, uint32_t bit_count);

void *heap_find_free_space(void *reserved, uint64_t size);

int header_find_free_bits_index(const uint8_t header[HEADER_SIZE], uint32_t bit_count);

void* entry_ptr_get(void *entry, uint32_t bit_index);

int entry_page_change_protection(void *entry, uint32_t bit_index, uint32_t bit_count, int protection);

int entry_page_protect(void *entry, uint32_t page_number, int protection, uint32_t pages_count);

// ------------------ PUBLIC FUNCTION IMPLEMENTATIONS ------------------

int heap_init(heap_t *self)
{
    self->destroy = heap_destroy;
    self->alloc = heap_alloc;
    self->free = heap_free;

    self->reserved = mmap(NULL, MAX_HEAP_SIZE, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    ASSERT_ERROR(NULL != self->reserved, "mmap couldn't reserve MAX_HEAP_SIZE bytes\n")

    int error = mprotect(self->reserved, HEADER_SIZE, PROT_WRITE);
    ASSERT_ERROR(!error, "mprotect couldn't commit a header of HEADER_SIZE bytes\n")

    return 0;
}

int heap_destroy(heap_t *self)
{
    int error = munmap(self->reserved, MAX_HEAP_SIZE);
    ASSERT_ERROR(!error, "munmap couldn't unmap the reserved address space of MAX_HEAP_SIZE bytes\n")

    self->reserved = NULL;

    return 0;
}

void* heap_alloc(heap_t *self, uint32_t size)
{
    ASSERT_ERROR_NULL(0 != size, "can not heap_alloc with size of 0\n")
    ASSERT_ERROR_NULL(ENTRY_SIZE - HEADER_SIZE >= size, "can not heap_alloc with size larger than ENTRY_SIZE - HEADER_SIZE\n")

    // check if first byte is a non-zero first byte
    if(!(((char*)self->reserved)[0]))
    {
        int error = entry_page_protect(self->reserved, 0, PROT_WRITE, 1);
        ASSERT_ERROR_NULL(!error, "couldn't commit the first page\n")

        ((char*)self->reserved)[0] = 1;
    }

    return heap_find_free_space(self->reserved, size);
}

int heap_free(heap_t *self, void *ptr, uint32_t size)
{
    ASSERT_ERROR(NULL != ptr, "heap_free trying to free a NULL pointer\n")
    ASSERT_ERROR(0 != size, "can not heap_free with size of 0\n")

    // find in which entry the ptr is
    uint32_t entry_number = ((uint64_t)ptr - (uint64_t)((char*)self->reserved + HEADER_SIZE)) >> 21;
    void *entry = (char*)self->reserved + entry_number * ENTRY_SIZE;

    // find at what index it is recorded
    uint32_t bit_index = ((uint64_t)ptr - (uint64_t)((char*)entry + HEADER_SIZE)) >> 6;

    // get bit_count
    uint32_t bit_count = ceil_p2(size, BLOCK_SIZE) >> 6;

    // free from index size elements
    int error = entry_header_decommit(entry, bit_index, bit_count);
    ASSERT_ERROR(!error, "couldn't decommit when freeing\n")

    return 0;
}

// ------------------ PRIVATE FUNCTION IMPLEMENTATIONS ------------------

uint64_t ceil_p2(uint64_t x, uint64_t p2)
{
    return x + p2 - 1 & -p2;
}

uint64_t floor_p2(uint64_t x, uint64_t p2)
{
    return x & -p2;
}

void* entry_ptr_get(void *entry, uint32_t bit_index)
{
    // - BLOCK_SIZE for the meta information in the header
    return (char*)entry + HEADER_SIZE + (bit_index - BLOCK_SIZE) * BLOCK_SIZE;
}

int entry_page_protect(void *entry, uint32_t page_number, int protection, uint32_t pages_count)
{
    int error = mprotect((char*)entry + HEADER_SIZE + PAGE_SIZE * page_number, PAGE_SIZE * pages_count, protection);
    ASSERT_ERROR(!error, "mprotect couldn't commit a page of size PAGE_SIZE on index page_number\n")

    return 0;
}

int heap_commit_new_entry(void *entry)
{
    int error = mprotect((char*)entry + ENTRY_SIZE, HEADER_SIZE, PROT_WRITE);
    ASSERT_ERROR(!error, "mprotect couldn't change protection to address entry + ENTRY_SIZE\n")

    return 0;
}

int entry_page_change_protection(void *entry, uint32_t bit_index, uint32_t bit_count, int protection)
{
    // get the ptr form current bit_index
    void *ptr = entry_ptr_get(entry, bit_index);
    ASSERT_ERROR(NULL != ptr, "entry_ptr_get got a NULL pointer\n")

    // find which page number we are in
    // >> 12 == / PAGE_SIZE
    uint32_t page_number = ((uint64_t)ptr - (uint64_t)((char*)entry + HEADER_SIZE)) >> 12;

    uint32_t pages_count = bit_count * BLOCK_SIZE >> 12;

    // TODO: tried ceil to 1; didn't work
    pages_count = pages_count ? pages_count - 1 : 1;

    // TODO: can you skip if page is already protected ?
    int error = entry_page_protect(entry, page_number, protection, pages_count);
    ASSERT_ERROR(!error, "entry_ptr_get got a NULL pointer\n")

    return 0;
}

void header_commit(const uint8_t header[HEADER_SIZE], uint32_t bit_index, uint32_t bit_count)
{
    uint32_t page_count = bit_count >> 6 ? (bit_count >> 6) - 1 : 0;
    uint32_t index = bit_index >> 3;

    uint32_t available_bits_count = __builtin_clzll(*(uint64_t*)(header + index)) + 1;

    if(bit_count <= available_bits_count)
    {
        *(uint64_t*)(header + index) |= ((1 << bit_count) - 1) << (bit_index & 63);
        return;
    }

    if(HEADER_PAGE_SIZE != available_bits_count)
    {
        *(uint64_t*)(header + index) |= ((1 << available_bits_count) - 1) << (bit_index & HEADER_PAGE_SIZE - 1);
        index += 8;
    }

    for(uint32_t i = 0; i < page_count; i++)
    {
        *(uint64_t*)(header + index) |= ~(uint64_t)0;
        index += 8;
    }

    uint32_t left_bits = bit_count - (available_bits_count + page_count * HEADER_PAGE_SIZE);
    *(uint64_t*)(header + index) |= (1 << left_bits) - 1;
}

void header_decommit(const uint8_t header[HEADER_SIZE], uint32_t bit_index, uint32_t bit_count)
{
    uint32_t page_count = bit_count / HEADER_PAGE_SIZE ? bit_count / HEADER_PAGE_SIZE - 1 : 0;
    uint32_t available_bits_count = __builtin_clzll(*(uint64_t*)(header + bit_index / 8)) + 1;

    uint32_t index = bit_index / 8;
    if(bit_count <= available_bits_count)
    {
        *(uint64_t*)(header + index) &= ~(((1 << bit_count) - 1) << (bit_index & HEADER_PAGE_SIZE - 1));
        return;
    }

    if(HEADER_PAGE_SIZE != available_bits_count)
    {
        *(uint64_t*)(header + index) &= ~(((1 << available_bits_count) - 1) << (bit_index & HEADER_PAGE_SIZE - 1));
        index += 8;
    }

    for(uint32_t i = 0; i < page_count; i++)
    {
        *(uint64_t*)(header + index) &= (uint64_t)0;
        index += 8;
    }

    uint32_t left_bits = bit_count - (available_bits_count + page_count * HEADER_PAGE_SIZE);
    *(uint64_t*)(header + index) &= ~((1 << left_bits) - 1);
}

int entry_header_commit(void *entry, uint32_t bit_index, uint32_t bit_count)
{
    // commit to memory if needed
    int error = entry_page_change_protection(entry, bit_index, bit_count, PROT_WRITE);
    ASSERT_ERROR(!error, "couldn't change protection to PROT_WRITE in a commit to an entry\n")

    // commit to header
    header_commit((uint8_t*)entry, bit_index, bit_count);

    return 0;
}

int entry_header_decommit(void *entry, uint32_t bit_index, uint32_t bit_count)
{
    // decommit from header
    header_decommit((uint8_t*)entry, bit_index, bit_count);

    // decommit page if needed
    int error = entry_page_change_protection(entry, bit_index, bit_count, PROT_NONE);
    ASSERT_ERROR(!error, "couldn't change protection to PROT_NONE in a decommit to an entry\n")

    return 0;
}

int header_find_free_bits_index(const uint8_t header[HEADER_SIZE], uint32_t bit_count)
{
    uint32_t page_count = bit_count >> 6 ? (bit_count >> 6) - 1 : 0;
    uint32_t bits_left = bit_count - page_count * HEADER_PAGE_SIZE;

    int64_t start;
    for(uint32_t iter = 64; iter < HEADER_SIZE * 8; iter += start + page_count * HEADER_PAGE_SIZE)
    {
        if(page_count)
        {
            start = -1;

            uint32_t cont = 0;
            for(uint32_t i = iter; i < HEADER_SIZE * 8 ; i += HEADER_PAGE_SIZE)
            {
                cont = *(uint64_t*)(header + (i >> 3)) & ~(uint64_t)0 ? 0 : cont + 1;
                if(cont == page_count)
                {
                    start = (int64_t)(i - (cont - 1) * HEADER_PAGE_SIZE);
                    break;
                }
            }
        }
        else start = iter;

        if(-1 == start) break;

        uint64_t left = page_count ?
                *(uint64_t*)(header + ((start - HEADER_PAGE_SIZE) >> 3)) ? __builtin_clzll(*(uint64_t*)(header + ((start - HEADER_PAGE_SIZE) >> 3))) : HEADER_PAGE_SIZE :
                *(uint64_t*)(header + (start >> 3)) ? __builtin_clzll(*(uint64_t*)(header + (start >> 3))) : HEADER_PAGE_SIZE;

        uint64_t new_index;
        if (HEADER_PAGE_SIZE != left)
        {
            new_index = page_count ? start + (HEADER_PAGE_SIZE - left) - HEADER_PAGE_SIZE :
                                     start + (HEADER_PAGE_SIZE - left);
        }
        else new_index = start;

        if (bits_left > left)
        {
            uint64_t right = page_count ? __builtin_ffsll(*(int64_t*)(header + ((start + HEADER_PAGE_SIZE * page_count) >> 3))) - 1 :
                                         __builtin_ffsll(*(int64_t*)(header + ((start + HEADER_PAGE_SIZE) >> 3))) - 1;
            right = -1 == right ? HEADER_PAGE_SIZE : right;
            if(bits_left - left > right)
                continue;
        }
        return (int)new_index;
    }
    return -1;
}

void *heap_find_free_space(void *reserved, uint64_t size)
{
    int error;

    uint32_t bit_count = ceil_p2(size, BLOCK_SIZE) >> 6;

    int bit_index = -1;
    void *entry = reserved;
    while(bit_index < 0)
    {
        bit_index = header_find_free_bits_index((uint8_t*)entry, bit_count);
        if(-1 == bit_index && !(((char*)entry)[1]))
        {
            error = heap_commit_new_entry(entry);
            ASSERT_ERROR_NULL(!error, "couldn't commit a new header for a new entry\n")

            ((char*)entry)[1] = 1;
            entry = (char*)entry + ENTRY_SIZE;
        }
        else if(-1 == bit_index)
        {
            entry = (char*)entry + ENTRY_SIZE;
        }
    }

    error = entry_header_commit(entry, bit_index, bit_count);
    ASSERT_ERROR_NULL(!error, "trying to commit to header failed\n")

    return entry_ptr_get(entry, bit_index);
}