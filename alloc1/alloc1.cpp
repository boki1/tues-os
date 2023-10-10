#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/mman.h>

#include "alloc1.h"

namespace alloc1 {

void bheader_mark_used(uint8_t *bheader, size_t chunk_index) {
	const size_t bm_byte = chunk_index >> 3;
	const size_t bm_byte_index = chunk_index & 0x7;
	bheader[bm_byte] |= 1 << bm_byte_index;
}

void block_header_mark_unused(uint8_t *bheader, size_t chunk_index) {
	const size_t bm_byte = chunk_index >> 3;
	const size_t bm_byte_index = chunk_index & 0x7;
	bheader[bm_byte] &= ~(1 << bm_byte_index);
}

bool bheader_is_last(uint8_t *bheader) {
	return *bheader & 0x80;
}

/*
 * The header of block contains a bitmap denoting whether the 64B
 * chunks inside the block are used or not. The first bit of the
 * bitmap marks the actual header, so it is reused as flagging
 * whether the block is last in the whole pool.
 */

inline size_t ceil_p2(size_t val, size_t p2) {
    return val + p2 - 1 & -p2;
}

std::optional<allocator::chunk_descriptor> allocator::fit_chunks(size_t chunks_needed) {
	// Plain first-fit search for the required amount of chunks.
	return {};
}

allocator::allocation allocator::malloc(size_t desired_size) {
	const size_t actual_size = ceil_p2(desired_size, CHUNK_SIZE);
	const size_t chunks_needed = actual_size >> 6; // effectively divides by CHUNK_SIZE

	size_t block_index, chunk_index;
	if (const auto desc = fit_chunks(chunks_needed)) {
	}

	uint8_t *selected_block_header = static_cast<uint8_t *>(m_pool) + BLOCK_HEADER_SIZE * block_index;
	bheader_mark_used(selected_block_header, chunk_index);
	if (!mprotect(selected_block_header, BLOCK_HEADER_SIZE, PROT_READ | PROT_WRITE))
		/* error */;

	struct allocation a = {
		.ptr = selected_block_header + BLOCK_HEADER_SIZE + (chunk_index << 6),
		.size = actual_size
	};

	return a;
}

allocator::allocation realloc(const allocator::allocation &allocated) {}

void free(const allocator::allocation &allocated) {}

}
