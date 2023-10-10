#ifndef ALLOC_H_INCLUDED_
#define ALLOC_H_INCLUDED_

#include <cstdio>
#include <stddef.h>
#include <sys/mman.h>

#include <algorithm>
#include <optional>

namespace alloc1 {

class allocator {
public:
	struct allocation {
		void *ptr{nullptr};
		size_t size{0};
	};

	struct stats {
		size_t num_allocations{0};
		size_t num_deallocations{0};
	};

	static constexpr size_t POOL_SIZE = 1 << 30;		// (nope) 2TB
	static constexpr size_t CHUNK_SIZE = 1 << 6; 			// 64B
	static constexpr size_t BLOCK_SIZE = 1 << 22;			// 2MB
	static constexpr size_t BLOCK_HEADER_SIZE = 1 << 12; 	// 4KB

public:
	allocator()
		: m_pool{mmap(NULL, POOL_SIZE, PROT_NONE, MAP_ANONYMOUS, -1, 0)} {
			if (m_pool == MAP_FAILED) {
				m_pool = nullptr;
				perror("mmap");
			}
		}

	allocator(const allocator &) = delete;
	allocator& operator=(const allocator &) = delete;

	allocator(allocator &&other) noexcept
		: m_pool{other.m_pool} {
		other.m_pool = nullptr;
	}

	allocator& operator=(allocator &&other) noexcept {
		std::swap(m_pool, other.m_pool);
		return *this;
	}

	~allocator() noexcept {
		if (m_pool && munmap(m_pool, POOL_SIZE))
			perror("munmap");
	}

	allocation malloc(size_t desired_size);
	allocation realloc(const allocation &allocated);
	void free(const allocation &allocated);

	size_t used() const;

private:
	struct chunk_descriptor {
		size_t block_index;
		size_t first_chunk_index;
		void *ptr;
	};

	std::optional<chunk_descriptor> fit_chunks(size_t chunks_needed);

private:
	void *m_pool;
	size_t m_used{0};
	stats m_stats;
};

}

#endif // ALLOC_H_INCLUDED_
