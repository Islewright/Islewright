#ifndef ISLEWRIGHT_COMMON_CHUNK_HPP
#define ISLEWRIGHT_COMMON_CHUNK_HPP

#include "islewright/common/constants.hpp"
#include "islewright/common/coords.hpp"
#include "islewright/common/tile.hpp"

#include <array>

namespace islewright::common {

// A dense kChunkW x kChunkH block of tiles owned by a single chunk entity.
// Storing one entity per chunk (rather than one per tile) keeps the registry
// small and iteration cache-friendly. Tiles are row-major; index them with
// local_index() from coords.hpp: tiles[local_index(t)] == tiles[ly * kChunkW + lx].
struct Chunk
{
    ChunkCoord coord;
    std::array<Tile, kChunkW * kChunkH> tiles;
};

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_CHUNK_HPP
