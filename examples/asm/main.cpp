#include <stdint.h>

struct myint {
	uint64_t v;
};

extern "C" {
	int f(int rcx);
	void swap(uint64_t *rdi, uint64_t *rsi);
	uint64_t g(uint64_t arr[], uint64_t index);
	uint64_t memcpy_dummy(char *dest, const char *src, uint64_t size);
	uint64_t bulk_sum(int *rdi, char rsi, uint64_t rdx, short rcx, int r8, int r9, int sp8, struct myint *);
}

int main(int argc, char *argv[]) {
	// auto rax = f(100);

	// uint64_t a = 100;
	// uint64_t b = 101;
	// swap(&a, &b);
	char arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	char arr2[100];
	// const uint64_t val = g(arr, 3);
	// memcpy_dummy(arr2, arr, 10);

	int i = 1;
	struct myint v {.v = 0x10000000};
	auto x = bulk_sum(&i, '\x10', 0x100000, 0x1000, 0x100, 0x1000000, 0x10000, &v);
	return 0;
}

