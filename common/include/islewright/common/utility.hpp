#ifndef ISLEWRIGHT_COMMON_UTILITY_HPP
#define ISLEWRIGHT_COMMON_UTILITY_HPP

namespace islewright::common {

// Floor division for a positive divisor, correct for negative dividends. Plain
// C++ '/' truncates toward zero, which would misplace tiles at negative
// coordinates into the wrong chunk.
constexpr int floor_div(int a, int b) noexcept
{
    const int q = a / b;
    const int r = a % b;
    return (r != 0 && (r < 0) != (b < 0)) ? q - 1 : q;
}

// Floor modulo for a positive divisor, correct for negative dividends. Plain
// C++ '%' follows truncation toward zero, so it can yield a negative remainder.
constexpr int floor_mod(int a, int b) noexcept
{
    const int r = a % b;
    return (r != 0 && (r < 0) != (b < 0)) ? r + b : r;
}

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_UTILITY_HPP
