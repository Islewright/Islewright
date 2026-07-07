#include "input.hpp"

namespace islewright::input {

void InputState::BeginFrame() noexcept
{
    m_keysPressed.fill(false);
    m_keysReleased.fill(false);
}

void InputState::HandleEvent(const SDL_Event& event) noexcept
{
    switch (event.type) {
    case SDL_EVENT_QUIT:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        m_quitRequested = true;
        break;
    case SDL_EVENT_KEY_DOWN:
        if (!event.key.repeat) {
            m_keysPressed[event.key.scancode] = !m_keysDown[event.key.scancode];
        }

        m_keysDown[event.key.scancode] = true;
        break;
    case SDL_EVENT_KEY_UP:
        m_keysReleased[event.key.scancode] = m_keysDown[event.key.scancode];
        m_keysDown[event.key.scancode] = false;
        break;
    default:
        break;
    }
}

bool InputState::QuitRequested() const noexcept
{
    return m_quitRequested;
}

} // namespace islewright::input
