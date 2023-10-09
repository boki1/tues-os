#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/mman.h>

#include "alloc.h"

/*
 * The header of block contains a bitmap denoting whether the 64B
 * chunks inside the block are used or not. The first bit of the
 * bitmap marks the actual header, so it is reused as flagging
 * whether the block is last in the whole pool.
 */
enum { BLOCK_HEADER_SIZE = 1 << 12 }; 	// 2MB
struct block_header {
	uint8_t raw[BLOCK_HEADER_SIZE];
};

void block_header_mark_used(struct block_header *bheader, size_t chunk_index) {
	const size_t bm_byte = chunk_index >> 3;
	const size_t bm_byte_index = chunk_index & 0x7;
	bheader->raw[bm_byte] |= 1 << bm_byte_index;
}

void block_header_mark_unused(struct block_header *bheader, size_t chunk_index) {
	const size_t bm_byte = chunk_index >> 3;
	const size_t bm_byte_index = chunk_index & 0x7;
	bheader->raw[bm_byte] &= ~(1 << bm_byte_index);
}

bool block_header_is_last(const struct block_header *bheader) {
	return bheader->raw[0] & 0x80;
}

static const size_t CHUNK_SIZE = 1 << 6; 			// 64B
static const size_t BLOCK_SIZE = 1 << 22;			// 2MB
static const size_t POOL_SIZE = 1 << 30; // 2TB (nope)

// TODO: Add multiple pools linked together so that in total it is 2TB.
struct allocator {
	void *pool;
};

int init_allocator(struct allocator *allocator) {
	void *res = mmap(NULL, POOL_SIZE, PROT_NONE, MAP_ANONYMOUS, -1, 0);
	if (res != MAP_FAILED) {
		allocator->pool = NULL;
		return 1;
	}
	allocator->pool = res;
	return 0;
}

int destroy_allocator(struct allocator *allocator) {
	if (allocator->pool)
		munmap(allocator->pool, POOL_SIZE);
	return 0;
}

size_t ceil_p2(size_t val, size_t p2) {
    return val + p2 - 1 & -p2;
}

int fit_chunks(size_t chunks_needed, size_t *block_index, size_t *block_inner_index) {
	// Plain first-fit search for the required amount of chunks.
	return 0;
}

struct allocation make_allocation(struct allocator *allocator, size_t desired_size) {
	const size_t actual_size = ceil_p2(desired_size, CHUNK_SIZE);
	const size_t chunks_needed = actual_size >> 6; // effectively divides by CHUNK_SIZE

	size_t block_index, chunk_index;
	if (!fit_chunks(chunks_needed, &block_index, &chunk_index))
		/* error */;

	struct block_header *selected_block_header = allocator->pool + sizeof(struct block_header) * block_index;
	block_header_mark_used(selected_block_header, chunk_index);
	if (!mprotect(selected_block_header, BLOCK_HEADER_SIZE, PROT_READ | PROT_WRITE))
		/* error */;

	struct allocation a = {
		.ptr = selected_block_header + BLOCK_HEADER_SIZE + (chunk_index << 6),
		.size = actual_size
	};

	return a;
}

struct allocation change_allocation(struct allocator *allocator, const struct allocation *allocated) {}

void free_allocation(struct allocator *allocator, const struct allocation *allocated) {}
