#ifndef ISLEWRIGHT_GAMELOOP_HPP
#define ISLEWRIGHT_GAMELOOP_HPP

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <thread>
#include <utility>

namespace islewright::gameloop {

class GameLoop
{
  public:
    using TickHandler = std::function<void(std::uint64_t)>;

    static constexpr std::uint32_t TickRate = 20;
    static constexpr auto TickInterval = std::chrono::milliseconds(1000 / TickRate);

    GameLoop() = default;
    GameLoop(const GameLoop&) = delete;
    GameLoop& operator=(const GameLoop&) = delete;

    ~GameLoop()
    {
        Stop();
    }

    void Start(TickHandler handler)
    {
        if (m_running.exchange(true)) {
            return;
        }

        m_thread = std::thread([this, handler = std::move(handler)]() { Run(handler); });
    }

    void Stop()
    {
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
            nextTick += TickInterval;
            std::this_thread::sleep_until(nextTick);
        }
    }
    
    std::atomic_bool m_running = false;
    std::thread m_thread;
};

} // namespace islewright::gameloop

#endif // ISLEWRIGHT_GAMELOOP_HPP
