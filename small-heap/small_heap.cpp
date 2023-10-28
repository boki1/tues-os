#include <cassert>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <stdexcept>

#include "small_heap.h"

/*
 * Helper functions
 */

std::size_t small_heap::ceil_p2(std::uintptr_t value, std::size_t p2) {
    return value + p2 - 1 & -p2;
}

std::size_t small_heap::floor_p2(std::uintptr_t value, std::size_t p2) {
    return value & -p2;
}

bool small_heap::is_last_block(void *bl_header) {
    return *reinterpret_cast<std::uint64_t *>(bl_header) != 0;
}

/*
 * When looking through the bitmap header, make sure that the first 64 bits
 * are skipped, as they mark the space which is actually occupied by the
 * header itself and there should be no allocations taking place there.
 */

std::optional<small_heap::chunk> small_heap::small_allocate(size_t count_pages, size_t count_chunks_in_last) {
    assert(count_chunks_in_last < CHUNKS_IN_BLOCK);
    // The number of pages in a block coincidentally is
    // equal to the number of chunks in a block.
    assert(count_pages < CHUNKS_IN_BLOCK);

    /*
     * This search tries to fit an allocation that is at least as big as PAGE_SIZE within
     * in a single block. Crucial for the implementation is the fact that the amount of bits
     * which describe the usage of a whole page is exactly uintptr_t big.
     */
    auto fit_in_block = [&](std::uintptr_t block_addr) -> ssize_t {
        ssize_t found_first_idx = -1;
        // The start is at 1 because the first uintptr_t is unused, since it corresponds to the
        // exact page which contains the bitmap.
        for (size_t found_count = 0, i = 1; i < CHUNKS_IN_BLOCK; ++i) {
            const std::uintptr_t bigword = *reinterpret_cast<std::uintptr_t *>(block_addr + i);
            if (found_count == count_pages) {
                if (!bigword || __builtin_clzll(bigword) > count_chunks_in_last)
                    return found_first_idx;
                found_first_idx = -1;
            }
            if (!bigword) {
                if (found_first_idx < 0)
                    found_first_idx = i;    // Safety: i < CHUNKS_IN_BLOCK so no loss could happen due to int type conversion.
                ++found_count;
                if (found_count == count_pages && count_chunks_in_last == 0)
                    return found_first_idx;
            }
        }

        assert(found_first_idx == -1);
        return found_first_idx;
    };

    auto mark_chunks_used = [count_pages, count_chunks_in_last](uintptr_t block_addr, size_t idx) {
        auto *bigword = reinterpret_cast<std::uintptr_t *>(block_addr + idx);
        for (size_t i = 0; i < count_pages; ++i, ++bigword) {
            assert(!*bigword);
            *bigword = ~0;
        }
        // Shifting uintptr_t by CHUNK_SIZE is UB.
        if (count_chunks_in_last == 0)
            return;
        const uintptr_t mask = static_cast<uintptr_t>(~0) << (CHUNK_SIZE - count_chunks_in_last);
        *bigword |= mask;
    };

    for (std::uintptr_t block_addr = m_reserved; block_addr < m_reserved_end; block_addr += BLOCK_SIZE) {
        TRY_OR (!mprotect(reinterpret_cast<void *>(block_addr), PAGE_SIZE, PROT_WRITE), "mprotect PROT_WRITE");
        if (const ssize_t idx_in_block = fit_in_block(block_addr); idx_in_block > 0) {
            mark_chunks_used(block_addr, idx_in_block);
            return chunk{.addr = block_addr + (idx_in_block << CHUNK_BITS),
                    .size = (count_chunks_in_last << CHUNK_BITS) + (count_pages << PAGE_BITS)};
        }
    }

    return {};
}

std::optional<small_heap::chunk> small_heap::tiny_allocate(size_t count_chunks) {
    assert(count_chunks < CHUNKS_IN_PAGE);

    auto fit_in_block_and_mark_used = [count_chunks](std::uintptr_t block_addr) -> ssize_t {
        const std::uintptr_t lsb_mask = ((1 << count_chunks) - 1);
        const std::uintptr_t mask = lsb_mask << (CHUNKS_IN_PAGE - count_chunks);
        for (size_t i = 1; i < CHUNKS_IN_BLOCK; ++i) {
            auto *bigword_ptr = reinterpret_cast<std::uintptr_t *>(block_addr + i);
            const std::uintptr_t bigword = *bigword_ptr;
            for (size_t j = 0; j < CHUNKS_IN_PAGE; ++j) {
                const std::uintptr_t bigword_shifted = bigword << j;
                if (~bigword_shifted & mask) {
                    *bigword_ptr |= mask;
                    return (i << CHUNK_BITS) + j;
                }
            }
        }
        return -1;
    };

    // Why is this false?
    // (And if so why does the mprotect() work?)
    // assert(block_addr == floor_p2(block_addr, PAGE_SIZE));

    for (std::uintptr_t block_addr = m_reserved; block_addr < m_reserved_end; block_addr += BLOCK_SIZE) {
        // Make the header of the block currently inspected usable.
        TRY_OR (!mprotect(reinterpret_cast<void *>(block_addr), PAGE_SIZE, PROT_WRITE), "mprotect PROT_WRITE");
        if (const ssize_t idx_in_block = fit_in_block_and_mark_used(block_addr); idx_in_block > 0) {
            const uintptr_t chunk_addr = block_addr + (idx_in_block << CHUNK_BITS);

            // Make the allocated space usable - PAGE_SIZE is used as in tiny_allocate the sizes of
            // the allocations are strictly less than the size of the page.
            const auto chunk_addr_aligned = floor_p2(chunk_addr, PAGE_SIZE);
            TRY_OR (!mprotect(reinterpret_cast<void *>(chunk_addr_aligned), PAGE_SIZE, PROT_WRITE), "mprotect PROT_WRITE");
            return chunk{.addr = chunk_addr, .size = count_chunks << CHUNK_BITS};
        }
    }
    return {};
}

std::optional<small_heap::chunk> small_heap::allocate(std::size_t desired_size) {
    const auto size = ceil_p2(desired_size, CHUNK_SIZE);
    if (desired_size > MAX_ALLOCATEBLE_SIZE)
        return {};

    // size is multiple of CHUNK_SIZE which is 2^{CHUNK_BITS} bytes.
    // Therefore, the number of chunks required is exactly size / CHUNK_SIZE, or
    const std::size_t count_chunks = size >> CHUNK_BITS;
    const std::size_t count_full_pages = floor_p2(count_chunks, CHUNKS_IN_PAGE) >> CHUNKS_IN_PAGE_BITS;
    const std::size_t count_in_last_page = count_chunks & (CHUNKS_IN_PAGE - 1);
    assert(count_in_last_page + count_full_pages * CHUNKS_IN_PAGE == count_chunks);

    return count_full_pages == 0
            ? tiny_allocate(count_in_last_page)
            : small_allocate(count_full_pages,count_in_last_page);
}

// TODO:
void free(small_heap::chunk &&allocated_chunk) {}