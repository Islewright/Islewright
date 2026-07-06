#ifndef ISLEWRIGHT_INPUT_HPP
#define ISLEWRIGHT_INPUT_HPP

#include <SDL3/SDL_events.h>

namespace islewright::input {

class InputState
{
  public:
    void BeginFrame() noexcept;
    void HandleEvent(const SDL_Event& event) noexcept;

    bool QuitRequested() const noexcept;
};

} // namespace islewright::input

#endif // ISLEWRIGHT_INPUT_HPP
