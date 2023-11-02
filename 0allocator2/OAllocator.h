
#ifndef INC_04_O_ALLOCATOR_OALLOCATOR_H
#define INC_04_O_ALLOCATOR_OALLOCATOR_H

#include <map>

#include "sallocator.h"

using std::size_t;

#ifndef ASSERT_ERROR
#define ASSERT_ERROR(expr, msg) if(!(expr)) { std::cerr << msg; exit(1);}
#define ASSERT_ERROR_NULL(expr, msg) if(!(expr)) { fprintf(stderr, msg); return NULL;}
#endif

#define MAX_SIZE ((size_t)1 << 46)

struct size_addr
{
    size_t size;
    void* addr;
};

bool operator<(const size_addr &l, const size_addr &r);

struct OAllocator
{
private:
    std::map<void*, size_t, std::less<>, SmallAllocator<std::pair<void*, size_t>>> addr2size;
    std::map<size_addr, void*, std::less<>, SmallAllocator<std::pair<size_addr, void*>>> size2addr;
    OAllocator();

public:
    OAllocator(const OAllocator&) = delete;
    OAllocator& operator=(const OAllocator&) = delete;

    static OAllocator& get_allocator();

    void* xalloc(size_t size);
    void xfree(void* ptr);
};

#endif //INC_04_O_ALLOCATOR_OALLOCATOR_H
