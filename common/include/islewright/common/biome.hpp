#ifndef ISLEWRIGHT_COMMON_BIOME_HPP
#define ISLEWRIGHT_COMMON_BIOME_HPP

#include <cstdint>

namespace islewright::common {

enum class BiomeType : std::uint8_t
{
    Plain = 0,
    Ocean,
    Forest,
    Mountain,
};

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_BIOME_HPP
