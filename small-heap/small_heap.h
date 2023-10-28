#ifndef SMALL_HEAP_H_INCLUDED
#define SMALL_HEAP_H_INCLUDED

#include <cstdint>
#include <cstdio>
#include <new>
#include <optional>
#include <sys/mman.h>
#include <unistd.h>

#define TRY_OR(expr, msg)                                          \
  ({                                                               \
    if (!(expr))                                                   \
      throw std::runtime_error(msg);                               \
  })

class small_heap {
public:
    static const inline std::size_t CHUNK_BITS = 6;
    static const inline std::size_t PAGE_BITS = 14;
    static const inline std::size_t BLOCK_BITS = 21;
    static const inline std::size_t CHUNKS_IN_PAGE_BITS = PAGE_BITS - CHUNK_BITS;

    static const inline std::size_t MAX_RESERVED_SIZE = 1ull << 41; // 2TB
    static const inline std::size_t PAGE_SIZE = 1 << PAGE_BITS;     // 4KB
    static const inline std::size_t CHUNK_SIZE = 1 << CHUNK_BITS;   // 64B
    static const inline std::size_t BLOCK_SIZE = 1 << BLOCK_BITS;   // 2MB

    // Effectively equivalent to PAGE_SIZE >> CHUNK_BITS or PAGE_SIZE / CHUNK_SIZE.
    static const inline std::size_t CHUNKS_IN_PAGE = 1 << CHUNKS_IN_PAGE_BITS;  // 256

    // Allocations are only within single blocks. As the size of the header is
    // exactly PAGE_SIZE, that leaves the following formula for maximum size of a
    // single allocation.
    static const inline std::size_t MAX_ALLOCATEBLE_SIZE = BLOCK_SIZE - PAGE_SIZE;
    static const inline std::size_t CHUNKS_IN_BLOCK = MAX_ALLOCATEBLE_SIZE >> CHUNK_BITS;

public: // Object lifetime
    small_heap() {
        void *reserved_ptr = mmap(0, MAX_RESERVED_SIZE, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (reserved_ptr == MAP_FAILED) {
            perror("mmap");
            throw std::bad_alloc{};
        }

        m_reserved = reinterpret_cast<std::uintptr_t>(reserved_ptr);
        m_reserved_end = m_reserved + MAX_RESERVED_SIZE;
    }

    // small_heap cannot be copied.
    small_heap(const small_heap &) = delete;
    small_heap &operator=(const small_heap &) = delete;

    // small_heap is movable.
    small_heap(small_heap &&) = default;
    small_heap &operator=(small_heap &&) = default;

public: // Public API.
    struct chunk {
        union {
            void *ptr;
            uintptr_t addr;
        };
        std::size_t size;
    };

    std::optional<chunk> allocate(std::size_t desired_size);
    void free(chunk &&allocated_chunk);

private: // Helper functions.
    static inline std::size_t ceil_p2(std::uintptr_t value, std::size_t p2);
    static inline std::size_t floor_p2(std::uintptr_t value, std::size_t p2);
    static inline bool is_last_block(void *bl_header);

    std::optional<chunk> tiny_allocate(size_t count_chunks);
    std::optional<chunk> small_allocate(size_t count_pages, size_t count_chunks_in_last);

private:
    std::uintptr_t m_reserved{0};
    std::uintptr_t m_reserved_end{0};
};

#endif // SMALL_HEAP_H_INCLUDED
