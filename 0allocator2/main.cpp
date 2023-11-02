#include <iostream>
#include "OAllocator.h"

#ifndef ASSERT_ERROR
#define ASSERT_ERROR(expr, msg) if(!(expr)) { perror(msg); exit(1);}
#define ASSERT_ERROR_NULL(expr, msg) if(!(expr)) { fprintf(stderr, msg); return NULL;}
#endif


int main(int argc, char **argv)
{
    int *ptr = (int*)OAllocator::get_allocator().xalloc(sizeof(int) * (1 << 30));
    ASSERT_ERROR(nullptr != ptr, "couldn't allocate with xalloc");

    for(int i = 0; i < 50; i++)
    {
        ptr[i] = i;
        std::cout << i << std::endl;
    }

    int *ptr2 = (int*)OAllocator::get_allocator().xalloc(sizeof(int) * (1 << 10));
    ASSERT_ERROR(nullptr != ptr2, "couldn't allocate with xalloc");

    for(int i = 50; i < 100; i++)
    {
        ptr2[i] = i;
        std::cout << i << std::endl;
    }

    OAllocator::get_allocator().xfree(ptr);
    OAllocator::get_allocator().xfree(ptr2);

    int *ptr3 = (int*)OAllocator::get_allocator().xalloc(sizeof(int) * (1 << 30));
    ASSERT_ERROR(nullptr != ptr3, "couldn't allocate with xalloc");

    for(int i = 100; i < 150; i++)
    {
        ptr3[i] = i;
        std::cout << i << std::endl;
    }

    OAllocator::get_allocator().xfree(ptr3);

    return 0;
}
