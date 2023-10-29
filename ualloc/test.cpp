#include <gtest/gtest.h>

#include "shalloc.h"
#include "ualloc.h"

TEST(Ualloc, TestOfTest) {
  EXPECT_STRNE("hello", "world");
  EXPECT_EQ(7 * 6, 42);

  ualloc::allocator a{1 << 12};
  EXPECT_EQ(a.page_size(), 1 << 10);
}
