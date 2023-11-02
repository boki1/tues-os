//
// Created by jivko on 11/1/23.
//

#ifndef INC_04_O_ALLOCATOR_SALLOCATOR_H
#define INC_04_O_ALLOCATOR_SALLOCATOR_H

#include <map>
#include <cstdint>
#include "small_allocator.h"

using std::size_t;

static heap_t heap;

template <typename T>
class SmallAllocator
{
private:

public:
    using value_type = T;

    [[nodiscard]] constexpr T*
    allocate(size_t n)
    {
        if(!heap.reserved)
            if (-1 == heap_init(&heap))
                return nullptr;

        return (T*)heap.alloc(&heap, n);
    }

    constexpr void deallocate(T* p, std::size_t n)
    {
        if(!heap.reserved || !p || !n) return;
        heap.free(&heap, p, n);
    }
};

#endif //INC_04_O_ALLOCATOR_SALLOCATOR_H
