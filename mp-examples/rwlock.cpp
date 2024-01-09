#include <atomic>
#include <cassert>
#include <cstring>

//#define DEBUG

constexpr bool is_dbg() {
#ifdef DEBUG
    return true;
#else
    return false;
#endif
}

#define assert_dbg(expr, msg) if constexpr (is_dbg()) if (!(expr)) { assert(!(msg)); }

struct rw_lock {
    bool lock_shared() {
        auto old_state = m_state.load(std::memory_order_acquire);
        if (old_state.exclusive)
            return false;
        state new_state = old_state;
        ++new_state.share_count;
        while (!m_state.compare_exchange_strong(old_state, new_state,
                                                std::memory_order_acq_rel, std::memory_order_acquire));
        return true;
    }

    void unlock_shared() {
        auto old_state = m_state.load(std::memory_order_acquire);
        assert_dbg(!old_state.exclusive && old_state.share_count > 0, "Bad unlock_shared() call.");
        state new_state = old_state;
        --new_state.share_count;
        while (!m_state.compare_exchange_strong(old_state, new_state,
                                                std::memory_order_acq_rel, std::memory_order_acquire));
    }

    bool lock_exclusive() {
        static const auto locked = [&] {
            state s{};
            std::memset(reinterpret_cast<uint8_t*>(&s), 0, sizeof(state));
            s.exclusive = true;
            return s;
        }();

        state expected_old = zero_state;
        return m_state.compare_exchange_strong(expected_old, locked,
                                               std::memory_order_acq_rel, std::memory_order_acquire);
    }

    void unlock_exclusive() {
#ifdef DEBUG
            auto old_state = m_state.load(std::memory_order_acquire);
            assert_dbg(old_state.exclusive && old_state.share_count == 0, "Bad unlock_exclusive() call");
#endif
        m_state.store(zero_state, std::memory_order_release);
    }

private:
    struct state {
        uint32_t share_count;
        bool exclusive;
        uint8_t padding[3];
    };

    static_assert(sizeof(state) == 8);
    static_assert(std::atomic<state>::is_always_lock_free);

    static const inline auto zero_state = [] {
        state s{};
        std::memset(reinterpret_cast<uint8_t*>(&s), 0, sizeof(state));
        return s;
    }();

    std::atomic<state> m_state{zero_state};
};

int main() {
    rw_lock rwl;
    auto lx = rwl.lock_exclusive();
    rwl.unlock_exclusive();

    auto ls = rwl.lock_shared();
    rwl.unlock_shared();

    rwl.lock_exclusive();
    auto ls_fails = rwl.lock_shared();
    assert(!ls_fails);
    rwl.unlock_exclusive();

    rwl.lock_shared();
    auto lx_fails = rwl.lock_exclusive();
    assert(!lx_fails);
}
