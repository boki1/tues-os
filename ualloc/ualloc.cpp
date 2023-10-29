#include <sys/mman.h>
#include <new>
#include <cassert>
#include <unistd.h>
#include <stdexcept>

#include "ualloc.h"

namespace ualloc {

bool operator<(const allocator::size_addr &lhs, const allocator::size_addr &rhs) noexcept {
	return lhs.size < rhs.size && lhs.addr < rhs.addr;
}

/*
 * Object lifetime
 */

allocator::allocator(std::size_t max_reserve_size)
	: m_max_reserve_size{max_reserve_size}
{
	const auto page_size= sysconf(_SC_PAGESIZE);
	if (page_size < 0)
		throw std::runtime_error{"sysconf(_SC_PAGESIZE) failed"};
	if ((max_reserve_size & page_size - 1) > 0)
		throw std::invalid_argument{"max_reserve_size is not page_size aligned"};
	void *ptr = mmap(0, max_reserve_size, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (ptr == MAP_FAILED)
		throw std::bad_alloc{};

	assert(m_free_blocks.empty());
	const size_addr first{
		.size = max_reserve_size,
		.addr = reinterpret_cast<std::uintptr_t>(ptr)
	};
	m_free_blocks[std::move(first)] = ptr;
}

/*
 * Allocator operations
 */

void *allocator::allocate(std::size_t target_size) {
	(void) target_size;
	return nullptr;
}

void allocator::free(void *ptr) noexcept {
	(void) ptr;
}

/*
 * Helper functions
 */

void allocator::split(allocator::size_addr &block, std::size_t target_size) {
	(void) block;
	(void) target_size;
}

void allocator::coalesce(std::uintptr_t addr) noexcept {
	(void) addr;
}

}
