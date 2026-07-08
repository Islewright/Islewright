#ifndef ISLEWRIGHT_CAMERA_HPP
#define ISLEWRIGHT_CAMERA_HPP

#include "math.hpp"

using namespace islewright;

namespace islewright::camera {

class Camera
{
  public:
    static constexpr float ZOOM_MIN = 0.1f;
    static constexpr float ZOOM_MAX = 8.0f;

  private:
    math::Vec2 m_position{};
    math::Vec2 m_viewport{};
    float m_zoom = 1.0f;
};

} // namespace islewright::camera

#endif // ISLEWRIGHT_CAMERA_HPP
