#ifndef ISLEWRIGHT_COMMON_BIOME_HPP
#define ISLEWRIGHT_COMMON_BIOME_HPP

#include <cstdint>

namespace islewright::common {

// The biome a tile belongs to. Drives world generation and, on the client
// side, appearance. Keep values stable and append new members at the end so
// existing saves/streams stay valid.
enum class BiomeType : std::uint8_t
{
    Ocean = 0,
    Plains,
    Forest,
    Mountain,
};

// Per-biome data. Minimal for Sprint 1; generation parameters can be added
// later. Plain aggregate so EnTT and containers can construct it freely.
struct Biome
{
    BiomeType type;

    bool operator==(const Biome&) const = default;
};

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_BIOME_HPP
