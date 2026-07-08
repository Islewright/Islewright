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

    // Getters
    constexpr math::Vec2 GetPosition() const noexcept
    {
        return m_position;
    }

    constexpr float Zoom() const noexcept
    {
        return m_zoom;
    }

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

    // Transform world coordinates to screen coordinates
    constexpr math::Vec2 WorldToScreen(math::Vec2 worldPos) const noexcept
    {
        return {.x = (worldPos.x - m_position.x) * m_zoom + m_viewport.x * 0.5f,
                .y = (worldPos.y - m_position.y) * m_zoom + m_viewport.y * 0.5f};
    }

    constexpr math::Vec2 ScreenToWorld(math::Vec2 screenPos) const noexcept
    {
        return {.x = (screenPos.x - m_viewport.x * 0.5f) / m_zoom + m_position.x,
                .y = (screenPos.y - m_viewport.y * 0.5f) / m_zoom + m_position.y};
    }

    // Zoom the camera at a specific screen position
    constexpr void ZoomAt(math::Vec2 screenPos, float zoomDelta) noexcept
    {
        math::Vec2 worldPosBeforeZoom = ScreenToWorld(screenPos);
        SetZoom(m_zoom + zoomDelta);
        math::Vec2 worldPosAfterZoom = ScreenToWorld(screenPos);

        m_position.x += worldPosBeforeZoom.x - worldPosAfterZoom.x;
        m_position.y += worldPosBeforeZoom.y - worldPosAfterZoom.y;
    }

  private:
    math::Vec2 m_position{};
    math::Vec2 m_viewport{};
    float m_zoom = 1.0f;
};

} // namespace islewright::camera

#endif // ISLEWRIGHT_CAMERA_HPP
