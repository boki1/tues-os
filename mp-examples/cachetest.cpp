#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono;
using namespace std::chrono_literals;

template <typename Fun, typename ...Args>
auto timeit(Fun &&fun, Args &&...args) {
	const auto start = high_resolution_clock::now();
	fun(std::forward<Args>(args)...);
	const auto end = high_resolution_clock::now();
	return duration_cast<milliseconds>(end - start);
}

void inc_ints(std::uint64_t *ints, std::uint64_t size) {
	static constexpr auto REPEATS = 1 << 12;
	for (std::uint64_t r = 0; r < REPEATS; ++r)
		for (std::uint64_t i = 0; i < size; ++i)
			++ints[i];
}

int main() {
	constexpr std::uint64_t MAX_COUNT = 1 << 18;
	constexpr std::uint64_t STEP_COUNT = 1 << 12;

	std::this_thread::sleep_for(500ms);

	std::uint64_t *ints = new std::uint64_t[MAX_COUNT];
	for (std::uint64_t count = STEP_COUNT;
			count < MAX_COUNT;
			count += STEP_COUNT) {
		const auto dur = timeit(inc_ints, ints, count);
		std::cout << "incs = " << count << ", "
					 "duration = " << dur.count() << "ms, "
					 "(" << static_cast<long double>(count) / dur.count() << " incs/ms)\n";
	}
	delete[] ints;

	return 0;
}
