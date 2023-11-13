#include <stdint.h>

extern "C" {
	int f(int rcx);
	void swap(uint64_t *rdi, uint64_t *rsi);
	uint64_t g(uint64_t arr[], uint64_t index);
}

int main(int argc, char *argv[]) {
	// auto rax = f(100);

	// uint64_t a = 100;
	// uint64_t b = 101;
	// swap(&a, &b);
	uint64_t arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	const uint64_t val = g(arr, 3);
	return 0;
}

