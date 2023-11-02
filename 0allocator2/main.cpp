#include <iostream>
#include "OAllocator.h"

#ifndef ASSERT_ERROR
#define ASSERT_ERROR(expr, msg) if(!(expr)) { perror(msg); exit(1);}
#define ASSERT_ERROR_NULL(expr, msg) if(!(expr)) { fprintf(stderr, msg); return NULL;}
#endif


int main(int argc, char **argv)
{
    int *ptr = (int*)OAllocator::get_allocator().xalloc(sizeof(int) * (1 << 10));

    for(int i = 0; i < 50; i++)
    {
        ptr[i] = i;
        std::cout << i << std::endl;
    }

    OAllocator::get_allocator().xfree(ptr);

    return 0;
}
