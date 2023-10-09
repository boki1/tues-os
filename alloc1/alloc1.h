#ifndef ALLOC_H_INCLUDED_
#define ALLOC_H_INCLUDED_

#include <stddef.h>

struct allocation {
	void *ptr;
	size_t size;
};

struct allocator;

int init_allocator(struct allocator *);
int destroy_allocator(struct allocator *);

struct allocation make_allocation(struct allocator *, size_t desired_size);
struct allocation change_allocation(struct allocator *, const struct allocation *allocated);
void free_allocation(struct allocator *, const struct allocation *allocated);

#endif // ALLOC_H_INCLUDED_
