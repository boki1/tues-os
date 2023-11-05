#ifndef UALLOC_H_INCLUDED
#define UALLOC_H_INCLUDED

#include <cstdint>
#include <map>

#include "shalloc.h"

namespace ualloc {

    class allocator {
    public:
        struct size_addr {
            std::size_t size;
            std::uintptr_t addr;
        };

    public:// Object lifetime.
        explicit allocator(std::size_t max_reserve_size);
        ~allocator() noexcept;

        allocator(const allocator&) = delete;
        allocator& operator=(const allocator&) = delete;

    public:// Allocator operations.
        [[nodiscard]] void* allocate(std::size_t target_size);

        // Deallocation guarantees success.
        void free(void* ptr, size_t size) noexcept;

    public:// Accessors
        [[nodiscard]] std::size_t page_size() const noexcept { return m_page_size; }

        [[nodiscard]] std::size_t max_reserve_size() const noexcept { return m_max_reserve_size; }

    private:// Members
        std::map<std::uintptr_t, std::size_t> m_addr2size;
        std::map<size_addr, void*> m_ordered;

        std::size_t m_page_size;
        const std::size_t m_max_reserve_size;
        void *m_reserved_ptr;
    };

    bool operator<(const allocator::size_addr& lhs, const allocator::size_addr& rhs) noexcept;

}// namespace ualloc

#endif// UALLOC_H_INCLUDED
