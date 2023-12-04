#include <cstdint>
#include <thread>
#include <memory>
#include <iostream>

class lock {
public:
	bool acquire();

	void acquire_strong() {
		while (acquire());
	}

	void release();

private:
	std::uint8_t m_val{0};
};

void dummy() {
	lock l;

	l.acquire_strong();
	l.release();

	// Hangs as expected :)
	l.acquire_strong();
	// l.acquire_strong();
	l.release();
}

int main() {
	// dummy();
	std::uint64_t counter{0};
	lock l;

	auto upcount = [&]() {
		for (int i = 0; i < 1000; ++i) {
			l.acquire_strong();
			std::atomic_thread_fence(std::memory_order_acquire);
			++counter;
			std::atomic_thread_fence(std::memory_order_release);
			l.release();
		}
	};

	std::thread t1{upcount};
	std::thread t2{upcount};

	t1.join(); t2.join();

	std::cout << "Counter: " << counter << '\n';

	return 0;
}
