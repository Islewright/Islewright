#ifndef ISLEWRIGHT_COMMON_TILE_HPP
#define ISLEWRIGHT_COMMON_TILE_HPP

#include "islewright/common/biome.hpp"

#include <cstdint>

namespace islewright::common {

enum class TileId : std::uint8_t
{
    Empty = 0,
    Grass,
    Water,
    Stone,
};

struct Tile
{
    TileId id;
    BiomeType biome;

    bool operator==(const Tile&) const = default;
};

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_TILE_HPP