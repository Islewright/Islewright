#ifndef ISLEWRIGHT_COMMON_COORDS_HPP
#define ISLEWRIGHT_COMMON_COORDS_HPP

#include "islewright/common/constants.hpp"
#include "islewright/common/utility.hpp"

namespace islewright::common {

// World-space tile coordinate on an integer grid. May be negative.
struct TileCoord
{
    int x;
    int y;

    bool operator==(const TileCoord&) const = default;
};

// Chunk-space coordinate. One chunk spans CHUNK_WIDTH x CHUNK_HEIGHT tiles.
struct ChunkCoord
{
    int x;
    int y;

    bool operator==(const ChunkCoord&) const = default;
};

// Maps a tile coordinate to the chunk that contains it.
constexpr ChunkCoord to_chunk(TileCoord tile) noexcept
{
    return ChunkCoord{floor_div(tile.x, CHUNK_WIDTH), floor_div(tile.y, CHUNK_HEIGHT)};
}

// Maps a tile coordinate to its packed index within its chunk's tile array:
// y * CHUNK_WIDTH + x, with x in [0, CHUNK_WIDTH) and y in [0, CHUNK_HEIGHT).
constexpr int local_index(TileCoord tile) noexcept
{
    const int x = floor_mod(tile.x, CHUNK_WIDTH);
    const int y = floor_mod(tile.y, CHUNK_HEIGHT);

    return y * CHUNK_WIDTH + x;
}

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_COORDS_HPP
