#include <cstdint>
#include <cassert>

template <typename T>
class atomic;

template <>
class atomic<std::int64_t> {
public:
	using Int = std::int64_t;

	atomic(Int initial = 0)
		: m_val{initial} {}

	operator Int() {
		return m_val;
	}

	Int operator++();
	Int operator--();
	Int operator++(int);
	Int operator--(int);

	Int fetch_add(Int arg);
	Int fetch_sub(Int arg);
	Int fetch_and(Int arg);
	Int fetch_or(Int arg);
	Int fetch_xor(Int arg);

	Int load() const;
	void store(Int arg);

	bool compare_exchange_weak(Int &expected, Int desired);

	void compare_exchange_strong(Int expected, Int desired) {
		while(!compare_exchange_weak(expected, desired))
			;
	}

private:
	Int m_val;
};

void test_simple() {
	/* Increment and decrement */

	{
		atomic<std::int64_t> a{42};
		auto next = ++a;
		assert(next == 43);
	}

	{
		atomic<std::int64_t> a{42};
		auto prev = a++;
		assert(prev == 42);
		assert(a == 43);
	}

	{
		atomic<std::int64_t> a{42};
		auto next = --a;
		assert(next == 41);
	}

	{
		atomic<std::int64_t> a{42};
		auto prev = a--;
		assert(prev == 42);
		assert(a == 41);
	}

	/* Fetch_* operations */

	{
		atomic<std::int64_t> a{0x100};
		auto prev = a.fetch_add(0x10);
		assert(prev == 0x100);
		assert(a == 0x110);
	}

	{
		atomic<std::int64_t> a{0x1100};
		auto prev = a.fetch_sub(0x100);
		assert(prev == 0x1100);
		assert(a == 0x1000);
	}

	{
		atomic<std::int64_t> a{0xf0f0};
		auto prev = a.fetch_and(0xff);
		assert(prev == 0xf0f0);
		assert(a == 0xf0);
	}

	{
		atomic<std::int64_t> a{0xf0f0};
		auto prev = a.fetch_or(0xff);
		assert(prev == 0xf0f0);
		assert(a == 0xf0ff);
	}

	{
		atomic<std::int64_t> a{0xf0f0};
		auto prev = a.fetch_xor(0x0ff0);
		assert(prev == 0xf0f0);
		assert(a == 0xff00);
	}

	/*
	 * Load and store
	 * 		- not really useful tests in single threaded environment
	 */

	{
		atomic<std::int64_t> a{0xaabb};
		assert(a.load() == 0xaabb);
		a.store(0xccddeeff);
		assert(a.load() == 0xccddeeff);
	}

	{
		atomic<std::int64_t> a{0xaabb};
		for (auto expected = a.load(); !a.compare_exchange_weak(expected, 0xccddeeff); )
			;
		assert(a.load() == 0xccddeeff);
	}

	{
		atomic<std::int64_t> a{0xaabb};
		auto expected = a.load();
		a.compare_exchange_strong(expected, 0xccddeeff);
		assert(a.load() == 0xccddeeff);
	}

	{
		atomic<std::int64_t> a{0xaabb};
		auto unexpected = a.load() - 17;
		bool failed_cas = !a.compare_exchange_weak(unexpected, 0xccddeeff);
		assert(failed_cas);
		assert(a.load() == 0xaabb);
	}

}

int main() {
	test_simple();
	return 0;
}
