#ifndef ISLEWRIGHT_COMMON_TILE_HPP
#define ISLEWRIGHT_COMMON_TILE_HPP

#include "islewright/common/biome.hpp"

#include <cstdint>

namespace islewright::common {

// Terrain/content kind of a single tile. Renderer-neutral: the client maps a
// TileId to a sprite/atlas region; the Core never stores render data. Keep
// values stable and append new members at the end.
enum class TileId : std::uint16_t
{
    Empty = 0,
    Grass,
    Water,
    Stone,
};

// A single map cell. Plain aggregate so EnTT and containers can construct it
// freely; holds only data (ids/enums), never SDL or pixel information.
struct Tile
{
    TileId id;
    BiomeType biome;

    bool operator==(const Tile&) const = default;
};

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_TILE_HPP
