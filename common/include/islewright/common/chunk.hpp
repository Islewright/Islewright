#ifndef ISLEWRIGHT_COMMON_CHUNK_HPP
#define ISLEWRIGHT_COMMON_CHUNK_HPP

#include "islewright/common/constants.hpp"
#include "islewright/common/coords.hpp"
#include "islewright/common/tile.hpp"

#include <array>

namespace islewright::common {

struct Chunk
{
    ChunkCoord coord;
    std::array<Tile, CHUNK_WIDTH * CHUNK_HEIGHT> tiles;
};

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_CHUNK_HPP
