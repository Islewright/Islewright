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

    bool QuitRequested() const noexcept
    {
        return m_quitRequested;
    }

    // Getters for keyboard state
    bool KeyDown(SDL_Scancode key) const noexcept
    {
        return ValidKey(key) && m_keysDown[key];
    }

    bool KeyPressed(SDL_Scancode key) const noexcept
    {
        return ValidKey(key) && m_keysPressed[key];
    }

    bool KeyReleased(SDL_Scancode key) const noexcept
    {
        return ValidKey(key) && m_keysReleased[key];
    }

    // Getters for mouse state
    bool MouseDown(Uint8 button) const noexcept
    {
        return ValidButton(button) && m_mouseDown[button - 1];
    }

    bool MousePressed(Uint8 button) const noexcept
    {
        return ValidButton(button) && m_mousePressed[button - 1];
    }

    bool MouseReleased(Uint8 button) const noexcept
    {
        return ValidButton(button) && m_mouseReleased[button - 1];
    }

    float MouseX() const noexcept
    {
        return m_mouseX;
    }

    float MouseY() const noexcept
    {
        return m_mouseY;
    }

    float MouseDeltaX() const noexcept
    {
        return m_mouseDeltaX;
    }

    float MouseDeltaY() const noexcept
    {
        return m_mouseDeltaY;
    }

    float WheelX() const noexcept
    {
        return m_wheelX;
    }

    float WheelY() const noexcept
    {
        return m_wheelY;
    }

    // Getters for window state
    bool WindowResized() const noexcept
    {
        return m_windowResized;
    }
    int WindowWidth() const noexcept
    {
        return m_windowWidth;
    }
    int WindowHeight() const noexcept
    {
        return m_windowHeight;
    }

  private:
    static constexpr std::size_t CNT_MOUSE_BUTTON_COUNT = 8;

    static constexpr bool ValidKey(SDL_Scancode key) noexcept
    {
        return key < SDL_SCANCODE_COUNT;
    }

    static constexpr bool ValidButton(Uint8 button) noexcept
    {
        return button > 0 && button <= CNT_MOUSE_BUTTON_COUNT;
    }

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
