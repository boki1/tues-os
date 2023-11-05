#include <algorithm>
#include <cassert>
#include <exception>
#include <new>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>

#include "ualloc.h"

#define TRY_OR(expr, msg)                  \
    ({                                     \
        if (!(expr))                       \
            throw std::runtime_error(msg); \
    })

#define MUST(expr, msg)                    \
    ({                                     \
        if (!(expr))                       \
            perror(msg), std::terminate(); \
    })

namespace ualloc {

    static inline uintptr_t as_uint(void* p)
    {
        return reinterpret_cast<uintptr_t>(p);
    }

    static inline void* as_voidp(uintptr_t p)
    {
        return reinterpret_cast<void*>(p);
    }

    bool operator<(const allocator::size_addr& lhs, const allocator::size_addr& rhs) noexcept
    {
        return lhs.size < rhs.size && lhs.addr < rhs.addr;
    }

    /*
 * Object lifetime
 */

    allocator::allocator(std::size_t max_reserve_size)
        : m_max_reserve_size{max_reserve_size}
    {
        const auto page_size = sysconf(_SC_PAGESIZE);
        if (page_size < 0)
            throw std::runtime_error{"sysconf(_SC_PAGESIZE) failed"};
        if ((max_reserve_size & page_size - 1) > 0)
            throw std::invalid_argument{"max_reserve_size is not page_size aligned"};
        m_reserved_ptr = mmap(0, max_reserve_size, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (m_reserved_ptr == MAP_FAILED)
            throw std::bad_alloc{};

        free(m_reserved_ptr, max_reserve_size);
    }

    allocator::~allocator() noexcept
    {
      MUST(!munmap(m_reserved_ptr, m_max_reserve_size), "munmap");
    }

    /*
 * Allocator operations
 */

    void* allocator::allocate(std::size_t target_size)
    {
        const auto first_fit_it = m_ordered.lower_bound({target_size});
        if (first_fit_it == m_ordered.end())
            return nullptr;

        const auto first_fit = first_fit_it->first;
        m_ordered.erase(first_fit_it);// Invalidates first_fit_it
        const auto count_erased = m_addr2size.erase(first_fit.addr);
        assert(count_erased == 1);

        if (first_fit.size > target_size) {
            size_addr entry{
                    .size = first_fit.size - target_size,
                    .addr = first_fit.addr + target_size};
            m_ordered[entry] = as_voidp(entry.addr);
            m_addr2size[entry.addr] = entry.size;
        }

       // TRY_OR(!mprotect(as_voidp(first_fit.addr), first_fit.size, PROT_WRITE), "mprotect PROT_WRITE");
        return as_voidp(first_fit.addr);
    }

    void allocator::free(void* ptr, size_t size) noexcept
    {
        size_addr entry{
                .size = size,
                .addr = as_uint(ptr)};

        if (const auto it = m_addr2size.upper_bound(entry.addr);
            it != m_addr2size.end() && entry.addr + entry.size == it->first) {
            m_ordered.erase(size_addr{
                    .size = it->second,
                    .addr = it->first});
            m_addr2size.erase(it->first);
            entry.size += it->second;
        }

        if (const auto it = m_addr2size.lower_bound(entry.addr);
            it != m_addr2size.end() && it->first + it->second == entry.addr) {
            m_ordered.erase(size_addr{
                    .size = it->second,
                    .addr = it->first});
            m_addr2size.erase(it->first);
            entry.addr = it->first;
            entry.size += it->second;
        }

        m_addr2size[entry.addr] = entry.size;
        m_ordered[entry] = as_voidp(entry.addr);
      //  MUST(!mprotect(as_voidp(entry.addr), entry.size, PROT_NONE), "mprotect PROT_NONE");
    }

}// namespace ualloc
