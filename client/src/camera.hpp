#ifndef ISLEWRIGHT_CAMERA_HPP
#define ISLEWRIGHT_CAMERA_HPP

#include "math.hpp"

#include <algorithm>

using namespace islewright;

namespace islewright::camera {

class Camera
{
  public:
    static constexpr float ZOOM_MIN = 0.1f;
    static constexpr float ZOOM_MAX = 8.0f;

    // Setters
    constexpr void SetPosition(math::Vec2 position) noexcept
    {
        m_position = position;
    }

    constexpr void SetViewportSize(float width, float height) noexcept
    {
        m_viewport = {.x = width, .y = height};
    }

    constexpr void SetZoom(float zoom) noexcept
    {
        m_zoom = std::clamp(zoom, ZOOM_MIN, ZOOM_MAX);
    }

  private:
    math::Vec2 m_position{};
    math::Vec2 m_viewport{};
    float m_zoom = 1.0f;
};

} // namespace islewright::camera

#endif // ISLEWRIGHT_CAMERA_HPP
