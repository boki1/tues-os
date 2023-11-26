#include <thread>
#include <iostream>
#include <atomic>

int main() {
	std::atomic<uint64_t> l = 1ull << 10, r = 0;
	auto fun = [&]() {
		while (l) --l, ++r;
	};
	std::thread t1{fun};
	std::thread t2{fun};
	t1.join();
	t2.join();

	std::cout << "l = " << l << '\n';
	std::cout << "r = " << r << '\n';
}
