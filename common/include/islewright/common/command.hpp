#ifndef ISLEWRIGHT_COMMON_COMMAND_HPP
#define ISLEWRIGHT_COMMON_COMMAND_HPP

#include "islewright/common/biome.hpp"
#include "islewright/common/coords.hpp"
#include "islewright/common/tile.hpp"

#include <variant>

namespace islewright::common {

//! Commands that can be applied to the world to place a tile at a specific coordinate.
struct PlaceTileCommand
{
    TileCoord coord;
    TileId tile;
};

//! Commands that can be applied to the world to set a biome at a specific coordinate.
struct SetBiomeCommand
{
    TileCoord coord;
    BiomeType biome;
};

using Command = std::variant<PlaceTileCommand, SetBiomeCommand>;

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_COMMAND_HPP
