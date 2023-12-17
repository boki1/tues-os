#include <atomic>
#include <cassert>
#include <cstring>

struct rwlock {

    bool try_lock_shared()
    {
        auto old_state = m_state.load(std::memory_order_acquire);
        if (old_state.exclusive)
            return false;
        state new_state = old_state;
        ++new_state.share_count;
        return m_state.compare_exchange_strong(old_state, new_state,
                                               std::memory_order_release, std::memory_order_acquire);
    }

    inline void lock_shared()
    {
        while (!try_lock_shared())
            ;
    }

    void unlock_shared()
    {
        auto old_state = m_state.load(std::memory_order_acquire);
        if (old_state.exclusive || old_state.share_count == 0)
            return;
        state new_state = old_state;
        --new_state.share_count;
        while (!m_state.compare_exchange_strong(old_state, new_state,
                                                std::memory_order_release, std::memory_order_acquire))
            ;
    }

    bool try_lock()
    {
        auto old_state = m_state.load(std::memory_order_acquire);
        if (old_state.exclusive || old_state.share_count > 0)
            return false;
        auto new_state = old_state;
        new_state.exclusive = true;
        return m_state.compare_exchange_strong(old_state, new_state,
                                               std::memory_order_release, std::memory_order_acquire);
    }

    inline void lock()
    {
        while (!try_lock())
            ;
    }

    void unlock()
    {
        auto old_state = m_state.load(std::memory_order_acquire);
        if (!old_state.exclusive || old_state.share_count == 0)
            return;
        auto new_state = old_state;
        new_state.exclusive = false;
        while (!m_state.compare_exchange_strong(old_state, new_state,
                                                std::memory_order_release, std::memory_order_acquire))
            ;
    }

    bool try_upgrade()
    {
        auto old_state = m_state.load(std::memory_order_acquire);
        if (old_state.exclusive || old_state.share_count > 1)
            return false;
        auto new_state = old_state;
        new_state.exclusive = true;
        new_state.share_count = 0;
        return m_state.compare_exchange_strong(old_state, new_state,
                                               std::memory_order_release, std::memory_order_acquire);
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
        state s;
        std::memset(reinterpret_cast<uint8_t*>(&s), 0, sizeof(state));
        return s;
    }();

    std::atomic<state> m_state{zero_state};
};

int main()
{
	rwlock l;
	l.lock_shared();
	l.unlock_shared();

	l.lock();
	l.unlock();
}
