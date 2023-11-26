#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

struct myint {
	uint64_t v;
};

struct vec2d {
	double x, y;
};

vec2d operator+(vec2d l, vec2d r);

extern "C" {
	int f(int rcx);
	void swap(uint64_t *rdi, uint64_t *rsi);
	uint64_t g(uint64_t arr[], uint64_t index);
	uint64_t memcpy_dummy(char *dest, const char *src, uint64_t size);
	uint64_t bulk_sum(int *rdi, char rsi, uint64_t rdx, short rcx, int r8, int r9, int sp8, struct myint *);
	uint64_t max(uint64_t rdi, uint64_t rsi);
	void add_many(uint64_t *res, const uint64_t *l, const uint64_t *r, uint64_t cnt);
	void sub_many(uint64_t *res, const uint64_t *l, const uint64_t *r, uint64_t cnt);
	void test();
}

extern "C" uint64_t zzz, const_zzz, zeroed_zzz;

struct ss {
};

ss operator+(const ss &l, const ss &r);

void test_addsub_many() {
//	uint64_t l1[] = { 0x1, 0x10, 0x100};
//	uint64_t r1[] = { 0x2, 0x20, 0x200};
//	uint64_t res1[3];
//	add_many(res1, l1, r1, 3);
//
//	uint64_t l2[] = {0,(uint64_t) -1};
//	uint64_t r2[] = {0,(uint64_t) -1};
//	uint64_t res2[2];
//	add_many(res2, l2, r2, 2);
//
//	uint64_t l3[] = { 0x10, (uint64_t) -1, (uint64_t) -1, (uint64_t) -1, (uint64_t) -1};
//	uint64_t r3[] = { 0x20, (uint64_t) -1, (uint64_t) -1, (uint64_t) -1, (uint64_t) -1};
//	uint64_t res3[5];
//	add_many(res3, l3, r3, 5);
//
//	uint64_t l4[] = { 0x4, 0x40, 0x400};
//	uint64_t r4[] = { 0x1, 0x10, 0x100};
//	uint64_t res4[3];
//	sub_many(res4, l4, r4, 3);

	uint64_t l5[] = { 0x7, 0x5, 0x4, 0x3, 0xffffffffffffffff};
	uint64_t r5[] = { 0x1, 0x2, 0x3, 0x4, 0x2};
	uint64_t res5[5];
	sub_many(res5, l5, r5, 5);
	uint64_t res52[5];
	add_many(res52, res5, r5, 5);
}

int main(int argc, char *argv[]) {
	// auto rax = f(100);

	// uint64_t a = 100;
	// uint64_t b = 101;
	// swap(&a, &b);
	// char arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	// char arr2[100];
	// const uint64_t val = g(arr, 3);
	// memcpy_dummy(arr2, arr, 10);

	// int i = 1;
	// struct myint v {.v = 0x10000000};
	// auto x = bulk_sum(&i, '\x10', 0x100000, 0x1000, 0x100, 0x1000000, 0x10000, &v);

	// const auto bigger1 = max(1, 3);
	// const auto bigger2 = max(42, 30);

	// test_addsub_many();
	// ss a, b;
	// a + b;
	//
	vec2d a {1, 2};
	vec2d b{3, 4};
	// auto c = a + b; FIXME:
	//

	++zzz;

	return 0;
}

