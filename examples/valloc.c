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

int main(int argc, char const *argv[])
{
    int *int_array = vallocate(5 * sizeof(int));
    if (NULL == int_array)
    {
        perror("Error from valloc");
        exit(1);
    }

    for(int *start = int_array, *end = int_array + 5; start != end; start++, *start = (unsigned long long)start % 69);

    for(int *start = int_array, *end = int_array + 5; start != end; start++) printf("%d ", *start);

    printf("\n");

    int_array = vreallocate(int_array, 6 * sizeof(int));

    if(NULL == int_array)
    {
        perror("Error from vrealloc");
        exit(1);
    }

    for(int *start = int_array, *end = int_array + 6; start != end;
            start++, *start = (unsigned long long)start % 69);

    for(int *start = int_array, *end = int_array + 6; start != end; start++)
            printf("%d ", *start);

    printf("\n");

    if (-1 == vdeallocate(int_array))
    {
        perror("Error from vfree");
        exit(1);
    }

    return 0;
}
