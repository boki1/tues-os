#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>

#define MAX_SIZE ((unsigned long long)1 << 46)

#ifndef MAP_ANONYMOUS
    #define MAP_ANONYMOUS 32
#endif

void *valloc_(unsigned long long sz) 
{
    // reserve a virtual address space
    void *ptr = mmap(NULL, MAX_SIZE, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (NULL == ptr)
    {
        fprintf(stderr, "mmap could't reserve %lld bytes", MAX_SIZE);
        return NULL;
    }

    // commit sz bytes to physical memory
    if (-1 == mprotect(ptr, sz, PROT_WRITE))
    {
        fprintf(stderr, "mprotect couldn't change the protection to write after VAS is reserved");
        return NULL;
    }

    return ptr;
}

int vfree(void *ptr)
{
    // release the reserved address space
    if (-1 == munmap(ptr, MAX_SIZE))
    {
        fprintf(stderr, "munmap couldn't remove the address space of size %lld bytes", MAX_SIZE);
        return -1;
    }

    return 0;
}

void *vrealloc(void *ptr, unsigned long long new_sz) 
{
    // commit the new size to the VAS    
    if (-1 == mprotect(ptr, new_sz, PROT_WRITE))
    {
        fprintf(stderr, "mpotect couldn't write %lld bytes to address space with WRITE protection", new_sz);
        return NULL;
    }

    // check for new_sz 
    if(new_sz >= MAX_SIZE) 
    {
        return ptr;
    }

    // decommiting the remaining address space
    if (-1 == mprotect(ptr + new_sz, MAX_SIZE - new_sz, PROT_NONE))
    {
        fprintf(stderr, "mpotect couldn't decommit the remaining address space %lld\n", MAX_SIZE - new_sz);
        return NULL;
    }

    return ptr;
}

int main(int argc, char const *argv[])
{
    int *int_array = valloc_(5 * sizeof(int));
    if (NULL == int_array) 
    {
        perror("Error from valloc");
        exit(1);
    }

    // init with random values
    for(int *start = int_array, *end = int_array + 5; start != end; 
            start++, *start = (unsigned long)start % 69);

    // print the values
    for(int *start = int_array, *end = int_array + 5; start != end; start++) 
            printf("%d ", *start);

    printf("\n");

    int_array = vrealloc(int_array, 6 * sizeof(int));

    if(NULL == int_array)
    {
        perror("Error from vrealloc");
        exit(1);
    }

    // init with random values
    for(int *start = int_array, *end = int_array + 6; start != end; 
            start++, *start = (unsigned long)start % 69);

    // print the values
    for(int *start = int_array, *end = int_array + 6; start != end; start++) 
            printf("%d ", *start);

    printf("\n");

    if (-1 == vfree(int_array))
    {
        perror("Error from vfree");
        exit(1);
    }

    return 0;
}
