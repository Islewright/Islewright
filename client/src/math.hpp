#ifndef ISLEWRIGHT_MATH_HPP
#define ISLEWRIGHT_MATH_HPP

namespace islewright::math {

struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;

    bool operator==(const Vec2&) const = default;
};

} // namespace islewright::math

#endif // ISLEWRIGHT_MATH_HPP
