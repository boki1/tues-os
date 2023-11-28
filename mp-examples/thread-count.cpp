#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>

/*
 * This program has effectively no need for splitting into multiple threads
 * due to the fact that the bottle neck isn't the CPU's capabilities.
 */

int main() {
	std::atomic<int64_t> l = 1ull << 30, r = 0;
	std::mutex m;
	auto fun = [&]() {
		std::unique_lock<std::mutex> _{m};
		while (l > 0)
			--l, ++r;
	};
	std::thread t1{fun};
	std::thread t2{fun};
	t1.join();
	t2.join();

	std::cout << "l = " << l << '\n';
	std::cout << "r = " << r << '\n';
}
