#include "input.hpp"

namespace islewright::input {

void InputState::BeginFrame() noexcept {}

void InputState::HandleEvent(const SDL_Event& event) noexcept {}

bool InputState::QuitRequested() const noexcept
{
    return false;
}

} // namespace islewright::input
