#include <chrono>
#include <iostream>
#include <vector>

using namespace std::chrono;

template <typename Fun, typename ...Args>
auto timeit(Fun &&fun, Args &&...args) {
	const auto start = high_resolution_clock::now();
	fun(std::forward<Args>(args)...);
	const auto end = high_resolution_clock::now();
	return duration_cast<microseconds>(end - start);
}

void inc_ints(std::uint64_t count, std::uint64_t *dest) {
	static constexpr auto REPEATS = 1 << 12;
	for (int i = 0; i < REPEATS; ++i)
		for (std::uint64_t j = 0; j < count; ++j)
			++dest[j];
}

int main() {
	constexpr std::uint64_t MAX_COUNT = 1 << 30;
	constexpr std::uint64_t STEP_COUNT = 1 << 12;

	std::uint64_t *dest = new std::uint64_t[MAX_COUNT];
	// std::fill_n(dest, MAX_COUNT, 0);
	for (std::uint64_t count = STEP_COUNT;
			count < MAX_COUNT;
			count += STEP_COUNT) {
		const auto dur = timeit(inc_ints, count, dest);
		std::cout << "size = " << (count >> 7) << "KB, "
					 "duration = " << dur.count() << "us, "
					 "(" << static_cast<long double>(count) / dur.count() << " incs/us)\n";
	}
	delete[] dest;

	return 0;
}
