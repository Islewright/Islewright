#include "input.hpp"

namespace islewright::input {

void InputState::BeginFrame() noexcept
{
    m_keysPressed.fill(false);
    m_keysReleased.fill(false);

    m_mousePressed.fill(false);
    m_mouseReleased.fill(false);

    m_mouseDeltaX = 0.0f;
    m_mouseDeltaY = 0.0f;
    m_wheelX = 0.0f;
    m_wheelY = 0.0f;

    m_windowResized = false;
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
    case SDL_EVENT_MOUSE_MOTION:
        m_mouseX = event.motion.x;
        m_mouseY = event.motion.y;
        m_mouseDeltaX += event.motion.xrel;
        m_mouseDeltaY += event.motion.yrel;
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button > 0 && event.button.button <= CNT_MOUSE_BUTTON_COUNT) {
            std::size_t index = static_cast<std::size_t>(event.button.button - 1);

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                m_mousePressed[index] = !m_mouseDown[index];
                m_mouseDown[index] = true;
            } else {
                m_mouseReleased[index] = m_mouseDown[index];
                m_mouseDown[index] = false;
            }
        }
        break;
    case SDL_EVENT_MOUSE_WHEEL:
        m_wheelX += event.wheel.x;
        m_wheelY += event.wheel.y;
        break;
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        m_windowResized = true;
        m_windowWidth = event.window.data1;
        m_windowHeight = event.window.data2;
        break;
    default:
        break;
    }
}

} // namespace islewright::input
