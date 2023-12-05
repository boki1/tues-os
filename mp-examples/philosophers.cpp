#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <ranges>
#include <semaphore>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>
using namespace std::chrono_literals;
using namespace std::string_view_literals;

static constexpr std::uint64_t g_philosopher_count = 5;
static constexpr std::array g_philosopher_names{"Socrates"sv, "Glaucon"sv, "Adeimantus"sv, "Polemarchus"sv, "Thrasymachus"sv};
static_assert(g_philosopher_names.size() == g_philosopher_count);

std::array g_forks{std::binary_semaphore{1}, std::binary_semaphore{1},
                   std::binary_semaphore{1}, std::binary_semaphore{1},
                   std::binary_semaphore{1}};

/*
 * Returns [smaller fork, bigger fork], i.e
 * if id == 0 -> [0, 1] and if id == 4 -> [0, 4].
 * This has the effect that *only one* of the philosophers is
 * left-handed whereas the other ones are right-handed.
 */
static inline constexpr auto ordered_forks(std::uint64_t id)
{
    if (id + 1 == g_philosopher_count)
        return std::make_pair((id + 1) % g_philosopher_count, id);
    return std::make_pair(id, (id + 1) % g_philosopher_count);
}

std::mutex g_cout_lock;
#define locked_cout(cout_expr)                       \
    {                                                \
        std::unique_lock<std::mutex> _{g_cout_lock}; \
        cout_expr;                                   \
    }

std::mt19937& random_engine()
{
    static std::random_device rd;
    static std::mt19937 e{rd()};
    return e;
}

struct philosopher {
    const std::uint64_t id;
    std::string_view name;

    void operator()()
    {
        for (;;) {
            think();
            pickup_forks();
            eat();
            putdown_forks();
            sleep();
        }
    }

    void think()
    {
        std::uniform_int_distribution<int> dist(1, 5);
        locked_cout(std::cout << name << " is thinking now.\n");
        std::this_thread::sleep_for(500ms * dist(random_engine()));
    }

    void eat()
    {
        std::uniform_int_distribution<int> dist(1, 5);
        locked_cout(std::cout << name << " is eating now.\n");
        std::this_thread::sleep_for(200ms * dist(random_engine()));
    }

    void sleep()
    {
        std::uniform_int_distribution<int> dist(1, 5);
        locked_cout(std::cout << name << " is sleeping now.\n");
        std::this_thread::sleep_for(350ms * dist(random_engine()));
    }

    void pickup_forks()
    {
        auto [l, r] = ordered_forks(id);
        assert(l != r);
        g_forks[l].acquire();
        g_forks[r].acquire();
    }

    void putdown_forks()
    {
        auto [l, r] = ordered_forks(id);
        assert(l != r);
        g_forks[l].release();
        g_forks[r].release();
    }
};

int main()
{
    std::cout << "\n ---- The Republic ----\n\n";
    ;
    std::vector<std::jthread> threads;
    threads.reserve(g_philosopher_count);
    for (auto [id, name]: std::views::enumerate(g_philosopher_names))
        threads.emplace_back([&](philosopher p) { p(); }, philosopher{std::uint64_t(id), name});

    return 0;
}
