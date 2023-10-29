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

public: // Object lifetime.
  explicit allocator(std::size_t max_reserve_size);

  allocator(const allocator &) = delete;
  allocator &operator=(const allocator &) = delete;

public: // Allocator operations.
  [[nodiscard]] void *allocate(std::size_t target_size);

  // Deallocation guarantees success.
  void free(void *ptr) noexcept;

private: // Helpers functions.
  /*
   * Given a block specified by a `size_addr`, split it so that the first
   * `target_size` of it is used and the other part is still available for
   * allocation.
   */
  void split(size_addr &block, std::size_t target_size);

  /*
   * Given an address which is available, try to coalesce it with the nearby
   * ones.
   */
  void coalesce(std::uintptr_t addr) noexcept;

public: // Accessors
  [[nodiscard]] std::size_t page_size() const noexcept { return m_page_size; }

  [[nodiscard]] std::size_t max_reserve_size() const noexcept { return m_max_reserve_size; }

private:
  std::map<std::uintptr_t, std::size_t, std::less<>,
           shalloc<std::pair<std::uintptr_t, std::size_t>>>
      m_used_blocks;

  std::map<size_addr, void *, std::less<>,
           shalloc<std::pair<const size_addr, void *>>>
      m_free_blocks;

  std::size_t m_page_size;
  const std::size_t m_max_reserve_size;
};

bool operator<(const allocator::size_addr &lhs, const allocator::size_addr &rhs) noexcept;

} // namespace ualloc

#endif // UALLOC_H_INCLUDED
