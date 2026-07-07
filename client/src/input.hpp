#ifndef ISLEWRIGHT_INPUT_HPP
#define ISLEWRIGHT_INPUT_HPP

#include <SDL3/SDL_events.h>
#include <array>

namespace islewright::input {

class InputState
{
  public:
    void BeginFrame() noexcept;
    void HandleEvent(const SDL_Event& event) noexcept;

    bool QuitRequested() const noexcept;

  private:
    static constexpr std::size_t CNT_MOUSE_BUTTON_COUNT = 8;

    bool m_quitRequested = false;

    std::array<bool, SDL_SCANCODE_COUNT> m_keysDown{};
    std::array<bool, SDL_SCANCODE_COUNT> m_keysPressed{};
    std::array<bool, SDL_SCANCODE_COUNT> m_keysReleased{};

    std::array<bool, CNT_MOUSE_BUTTON_COUNT> m_mouseDown{};
    std::array<bool, CNT_MOUSE_BUTTON_COUNT> m_mousePressed{};
    std::array<bool, CNT_MOUSE_BUTTON_COUNT> m_mouseReleased{};

    float m_mouseX = 0.0f;
    float m_mouseY = 0.0f;
    float m_mouseDeltaX = 0.0f;
    float m_mouseDeltaY = 0.0f;
    float m_wheelX = 0.0f;
    float m_wheelY = 0.0f;

    bool m_windowResized = false;
    int m_windowWidth = 0;
    int m_windowHeight = 0;
};

} // namespace islewright::input

#endif // ISLEWRIGHT_INPUT_HPP
