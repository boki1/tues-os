#include <stdint.h>

extern "C" {
	int f(int rcx);
	void swap(uint64_t *rdi, uint64_t *rsi);
	uint64_t g(uint64_t arr[], uint64_t index);
	uint64_t memcpy_dummy(char *dest, const char *src, uint64_t size);
}

int main(int argc, char *argv[]) {
	// auto rax = f(100);

	// uint64_t a = 100;
	// uint64_t b = 101;
	// swap(&a, &b);
	char arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	char arr2[100];
	// const uint64_t val = g(arr, 3);
	memcpy_dummy(arr2, arr, 10);
	return 0;
}

