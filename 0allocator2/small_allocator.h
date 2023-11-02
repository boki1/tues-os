#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstdint>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 32
#endif // MAP_ANONYMOUS

#define MAX_HEAP_SIZE ((uint64_t)1 << 46) // 64 TB
#define ENTRY_SIZE ((uint64_t)1 << 21) // 2MB
#define HEADER_SIZE ((uint64_t)1 << 12) // 4KB
#define PAGE_SIZE ((uint64_t)1 << 12) // 4KB
#define HEADER_PAGE_SIZE ((uint64_t)1 << 6) // 64B
#define BLOCK_SIZE ((uint64_t)1 << 6) // 64B

typedef struct heap_t
{
    void *reserved;

    int (*destroy)(struct heap_t *self);
    void* (*alloc)(struct heap_t *self, uint32_t size);
    int (*free)(struct heap_t *self, void *ptr, uint32_t size);
} heap_t;

int heap_init(heap_t *self);

int heap_destroy(heap_t *self);

void* heap_alloc(heap_t *self, uint32_t size);

int heap_free(heap_t *self, void *ptr, uint32_t size);

#endif // ALLOCATOR_H