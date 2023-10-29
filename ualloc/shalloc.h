#ifndef UALLOC_SHALLOC_H
#define UALLOC_SHALLOC_H

#include <cstdint>

/*
 * Small-heap allocator.
 * Used for allocations of no more than 2032Kbytes (per query).
 * Requiered by the ualloc in order to allocate memory for the
 * internal std::map<> nodes.
 */
template <typename T>
class shalloc {
public:
	using value_type = T;

	[[nodiscard]] constexpr T* allocate(std::size_t n) {return nullptr;}

	constexpr void deallocate(T* p, std::size_t n) {}
};

#endif // UALLOC_SHALLOC_H
