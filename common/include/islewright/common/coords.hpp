#pragma once

namespace islewright::common {

// Dimensions of a single chunk in tiles. Chunk-local indices span
// [0, kChunkW) x [0, kChunkH). These live with the coordinate system because
// the tile <-> chunk conversions below depend on them; chunk.hpp reuses them.
inline constexpr int kChunkW = 32;
inline constexpr int kChunkH = 32;

// World-space tile coordinate on an integer grid. May be negative.
struct TileCoord
{
    int x;
    int y;

    bool operator==(const TileCoord&) const = default;
};

// Chunk-space coordinate. One chunk spans kChunkW x kChunkH tiles.
struct ChunkCoord
{
    int cx;
    int cy;

    bool operator==(const ChunkCoord&) const = default;
};

namespace detail {

// Floor division and modulo for a positive divisor, correct for negative
// dividends. Plain C++ '/' and '%' truncate toward zero, which would misplace
// tiles at negative coordinates into the wrong chunk.
constexpr int floor_div(int a, int b) noexcept
{
    const int q = a / b;
    const int r = a % b;
    return (r != 0 && (r < 0) != (b < 0)) ? q - 1 : q;
}

constexpr int floor_mod(int a, int b) noexcept
{
    const int r = a % b;
    return (r != 0 && (r < 0) != (b < 0)) ? r + b : r;
}

} // namespace detail

// Maps a tile coordinate to the chunk that contains it.
constexpr ChunkCoord to_chunk(TileCoord tile) noexcept
{
    return ChunkCoord{detail::floor_div(tile.x, kChunkW), detail::floor_div(tile.y, kChunkH)};
}

// Maps a tile coordinate to its packed index within its chunk's tile array:
// ly * kChunkW + lx, with lx in [0, kChunkW) and ly in [0, kChunkH).
constexpr int local_index(TileCoord tile) noexcept
{
    const int lx = detail::floor_mod(tile.x, kChunkW);
    const int ly = detail::floor_mod(tile.y, kChunkH);
    return ly * kChunkW + lx;
}

} // namespace islewright::common
