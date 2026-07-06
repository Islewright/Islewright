#include "input.hpp"

namespace islewright::input {

void InputState::BeginFrame() noexcept {}

void InputState::HandleEvent(const SDL_Event& event) noexcept
{
    switch (event.type) {
    case SDL_EVENT_QUIT:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        m_quitRequested = true;
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
