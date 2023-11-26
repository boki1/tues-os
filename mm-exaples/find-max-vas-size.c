#include <stdio.h>
#include <sys/mman.h>

#ifndef MAP_ANONYMOUS
    #define MAP_ANONYMOUS 32
#endif

int main(int argc, char const *argv[])
{
    void *ptr;
    size_t max_size = 0;

    for(size_t i = 1; i < 70; i++)
    {
        ptr = mmap(NULL, (unsigned long long)1 << i, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        
        if(NULL == ptr)
        {
            perror("couldn't mmap");
            return 1;
        }

        if(-1 == mprotect(ptr, 1, PROT_WRITE))
        {
            max_size = i - 1;
            break;
        }
        if(-1 == munmap(ptr, (unsigned long long)1 << i))
        {
            perror("couldn't munmap");
            return 1;
        }
    }

    printf("Max size = 1 << %ld\n", max_size);

    return 0;
}
