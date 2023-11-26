#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

// This value is calculated using the find-max-vas-size.c example
#define MAX_SIZE ((uint64_t)1 << 46)

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 32
#endif

/*
 * Helpers.
 */

#define MUST(condition_expr, on_error_expr)                                    \
  if (!(condition_expr)) {                                                     \
    on_error_expr;                                                             \
    goto error;                                                                \
  }

uint64_t floor_p2(uint64_t x, uint64_t p2) { return x & -p2; }

uint64_t ceil_p2(uint64_t x, uint64_t p2) { return x + p2 - 1 & -p2; }

/*
 * Caches the value returned by the first successful sysconf(_SC_PAGESIZE).
 */
static ssize_t page_size() {
  static ssize_t _page_size = -1;
  if (_page_size == -1)
    _page_size = sysconf(_SC_PAGESIZE);
  return _page_size;
}

/*
 * Allocator operations.
 */

void *vallocate(uint64_t sz) {
  const ssize_t page = page_size();
  MUST(page > 0, perror("page_size"));

  // Allocation sizes are aligned to the page size.
  sz = ceil_p2(sz, page);

  // Reserve MAX_SIZE regardless of the desired size in order to not have to
  // move memory on reallocation.
  void *ptr =
      mmap(NULL, MAX_SIZE, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  MUST(ptr != MAP_FAILED, perror("mmap"));

  // However commit only the desired amount to physical memory.
  MUST(!mprotect(ptr, sz, PROT_WRITE), perror("mprotect"));

  return ptr;

  // Jumps from MUST() come here.
error:
  return NULL;
}

int vdeallocate(void *ptr) {
  MUST(munmap(ptr, MAX_SIZE) != -1, perror("munmap"));
  return 0;

  // Jump from MUST() come here.
error:
  return -1;
}

void *vreallocate(void *ptr, uint64_t new_sz) {
  const ssize_t page = page_size();
  MUST(page > 0, perror("page_size"));

  // Allocation sizes are aligned to the page size.
  new_sz = ceil_p2(new_sz, page);
  MUST(new_sz <= MAX_SIZE,
       fprintf(stderr, "new_sz is bigger than the max size: %lu", MAX_SIZE));

  MUST(!mprotect(ptr, new_sz, PROT_WRITE), perror("mprotect"));

  // Decommit any unused memory.
  if (new_sz < MAX_SIZE) {
    const uintptr_t decommit_addr = ceil_p2((uint64_t)ptr + new_sz, page);
    const uint64_t pad_sz = decommit_addr - (uint64_t)ptr;
    MUST(
        !mprotect((void *)decommit_addr, MAX_SIZE - new_sz + pad_sz, PROT_NONE),
        perror("mprotect"));
  }

  return ptr;

error:
  return NULL;
}

/*
 * Simple test case with array of integers.
 */

void intset(int *s, int c, size_t n) {
  for (int *p = s; p < s + n; ++p)
    *p = c;
}

int main(int argc, char const *argv[]) {
  const size_t numbytes = 5 * sizeof(int);
  int *ptr = vallocate(numbytes);
  intset(ptr, 0xA, numbytes);
  assert(ptr != NULL);
  assert(ptr[3] == 0xA);

  const size_t big_new_size = 1 << 10;
  int *big_new_ptr = vreallocate(ptr, big_new_size);
  assert(big_new_ptr != NULL);
  assert(big_new_ptr == ptr);
  intset(ptr, 0xB, big_new_size);
  assert(big_new_ptr[1000] == 0xB);
  big_new_ptr[1000] = 0xBB;
  assert(big_new_ptr[1000] == 0xBB);

  const size_t small_new_size = 10;
  int *small_new_ptr = vreallocate(ptr, small_new_size);
  assert(small_new_ptr != NULL);
  assert(small_new_ptr == ptr);
  intset(ptr, 0xC, small_new_size);
  assert(small_new_ptr[8] == 0xC);
  small_new_ptr[8] = 0xCC;
  assert(small_new_ptr[8] == 0xCC);

  // FIXME
#if 0
  const size_t large_new_size = MAX_SIZE - 1;
  int *large_new_ptr = vreallocate(ptr, large_new_size);
  assert(large_new_ptr != NULL);
  assert(large_new_ptr == ptr);
  intset(ptr, 0xD, large_new_size);
  assert(large_new_ptr[121341] == 0xD);
  large_new_ptr[121341] = 0xDD;
  assert(large_new_ptr[121341] == 0xDD);
#endif

  const int rc_deallocate = vdeallocate(ptr);
  assert(!rc_deallocate);

  printf("OK - all assertions succeed.\n");
  return 0;
}
