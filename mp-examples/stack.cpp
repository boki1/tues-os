#include <atomic>
#include <stdexcept>
#include <memory>
#include <thread>
#include <random>
#include <chrono>
using namespace std::chrono_literals;

struct stack {
    void push(int data) {
        item *new_item;
        auto old_state = m_state.load(std::memory_order_acquire);
        state new_state{};
        do {
            new_item = new item{
                    .next = old_state.top,
                    .data=data};
            new_state = {
                    .top = new_item,
                    .below_top = new_item->next
            };
        } while (!m_state.compare_exchange_strong(old_state, new_state,
                                                  std::memory_order_acq_rel,
                                                  std::memory_order_acquire));
    }

    int pop() {
        item *old_item;
        auto old_state = m_state.load(std::memory_order_acquire);
        state new_state{};
        do {
            if (!old_state.top) throw std::runtime_error{"no elements in stack"};
            old_item = old_state.top;
            new_state = {
                    .top = old_item->next,
                    .below_top = old_item->next ? old_item->next->next : nullptr
            };
        } while (!m_state.compare_exchange_strong(old_state, new_state,
                                                  std::memory_order_acq_rel,
                                                  std::memory_order_acquire));
        std::unique_ptr<item> old_item_ptr{old_item};
        return old_item_ptr->data;
    }

    struct item {
        item *next;
        int data;
    };

    struct state {
        item *top, *below_top;
    };

    std::atomic<state> m_state;
};

int main() {
    stack s;
    std::random_device rand_device;
    std::mt19937 rand_engine(rand_device());

    std::jthread producer{[&] {
        std::uniform_int_distribution<int> rand_distrib_st(1, 5);
        std::uniform_int_distribution<int> rand_distrib_data(1, 10000);
        for (;;) {
            const auto new_data = rand_distrib_data(rand_engine);
            s.push(new_data);
            std::this_thread::sleep_for(50ms * rand_distrib_st(rand_engine));
        }
    }};

    std::jthread consumer{[&] {
        std::uniform_int_distribution<int> rand_distrib_st(1, 5);
        for (;;) {
            std::this_thread::sleep_for(100ms * rand_distrib_st(rand_engine));
            try { const auto old_data = s.pop();}
            catch (std::runtime_error &) {}
        }
    }};

    return 0;
}
