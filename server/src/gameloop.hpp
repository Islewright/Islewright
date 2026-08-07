#ifndef ISLEWRIGHT_GAMELOOP_HPP
#define ISLEWRIGHT_GAMELOOP_HPP

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <thread>
#include <utility>
#include <mutex>

namespace islewright::gameloop {

class GameLoop
{
  public:
    using TickHandler = std::function<void(std::uint64_t)>;

    static constexpr std::uint32_t TICK_RATE = 20;
    static constexpr auto TICK_INTERVAL = std::chrono::milliseconds(1000 / TICK_RATE);

    GameLoop() = default;
    GameLoop(const GameLoop&) = delete;
    GameLoop& operator=(const GameLoop&) = delete;
    GameLoop(GameLoop&&) = delete;
    GameLoop& operator=(GameLoop&&) = delete;

    ~GameLoop()
    {
        Stop();
    }

    void Start(TickHandler handler)
    {
        std::lock_guard<std::mutex> lock(m_lifecycleMutex);
        if (m_running.exchange(true)) {
            return;
        }

        m_thread = std::thread([this, handler = std::move(handler)]() { Run(handler); });
    }

    void Stop()
    {
        std::lock_guard<std::mutex> lock(m_lifecycleMutex);
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    bool IsRunning() const noexcept
    {
        return m_running;
    }

  private:
    void Run(const TickHandler& handler)
    {
        using Clock = std::chrono::steady_clock;

        std::uint64_t tick = 0;
        auto nextTick = Clock::now();

        while (m_running) {
            handler(tick++);
            nextTick += TICK_INTERVAL;
            const auto now = Clock::now();
            if(nextTick < now)
            {
                nextTick = now;
                continue;
            }
            std::this_thread::sleep_until(nextTick);
        }
    }
    
    std::atomic_bool m_running = false;
    std::thread m_thread;
    std::mutex m_lifecycleMutex;
};

} // namespace islewright::gameloop

#endif // ISLEWRIGHT_GAMELOOP_HPP
