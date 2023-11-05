#include <gtest/gtest.h>

#include "shalloc.h"
#include "ualloc.h"

static inline uintptr_t as_uint(void* p)
{
    return reinterpret_cast<uintptr_t>(p);
}

static inline void* as_voidp(uintptr_t p)
{
    return reinterpret_cast<void*>(p);
}

TEST(Ualloc, TestBasics)
{
    ualloc::allocator a{1 << 12};// 4KB heap
    EXPECT_EQ(a.max_reserve_size(), 1 << 12);

    void* p13 = a.allocate(13);
    EXPECT_NO_THROW(a.free(p13, 13));
    void* p14 = a.allocate(14);
    EXPECT_NO_THROW(a.free(p14, 14));
    EXPECT_EQ(as_uint(p13), as_uint(p14));
}

TEST(Ualloc, TestCoalescingToTheLeft)
{
    ualloc::allocator a{1 << 12};// 4KB heap
    void* p13 = a.allocate(13);
    void* p14 = a.allocate(14);
    void* p442 = a.allocate(442);

    // Should coalesce to the "left"
    EXPECT_NO_THROW(a.free(p14, 14));
    EXPECT_NO_THROW(a.free(p13, 13));

// FIXME:
//    void* p25 = a.allocate(25);
//    EXPECT_EQ(as_uint(p25), as_uint(p13));
}

TEST(Ualloc, TestCoalescingBothSides)
{
    ualloc::allocator a{1 << 12};// 4KB heap
    void* p13 = a.allocate(13);
    void* p14 = a.allocate(14);
    void* p442 = a.allocate(442);

    EXPECT_NO_THROW(a.free(p14, 14));
    // Should coalesce both sides.
    EXPECT_NO_THROW(a.free(p442, 442));

// FIXME:
//    void* p600 = a.allocate(600);
//    EXPECT_EQ(as_uint(p600), as_uint(p14));
}

TEST(Ualloc, TestNomem)
{


// FIXME:ualloc::allocator a{1 << 12};// 4KB heap
//    void* p = a.allocate((1 << 12) - 1);
//	EXPECT_NE(p, nullptr);

	void *p14;
	EXPECT_NO_THROW(p14 = a.allocate(14));
	EXPECT_EQ(p14, nullptr);
}

TEST(Ualloc, TestBadFree)
{
    ualloc::allocator a{1 << 12};// 4KB heap
												  //
	// Try to free address which hasn't been allocated.
	a.free(as_voidp(0xABCD), 13);

	// Double-free
	void *p13 = a.allocate(13);
	EXPECT_NE(p13, nullptr);
	EXPECT_NO_THROW(a.free(p13, 13));
	EXPECT_NO_THROW(a.free(p13, 13));
}
