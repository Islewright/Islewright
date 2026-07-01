#ifndef ISLEWRIGHT_COMMON_RNG_HPP
#define ISLEWRIGHT_COMMON_RNG_HPP

#include "islewright/common/coords.hpp"

#include <cstdint>

namespace islewright::common {

// SplitMix64 finalizer: a cheap, well-distributed avalanche over a 64-bit input.
constexpr std::uint64_t splitmix64(std::uint64_t x) noexcept
{
    x += 0x9E3779B97F4A7C15ULL;
    x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ULL;
    x = (x ^ (x >> 27)) * 0x94D049BB133111EBULL;
    return x ^ (x >> 31);
}

// Derives a deterministic 64-bit value from a world seed and chunk coordinates,
// suitable for seeding a per-chunk RNG. Order-sensitive in (cx, cy):
// hash_coord(s, {1, 2}) != hash_coord(s, {2, 1}). Taking a ChunkCoord instead
// of two loose ints stops call sites from silently swapping the axes. World
// generation must seed RNGs from coordinates this way, never from entity ids or
// iteration order, so the same seed always reproduces the same world.
constexpr std::uint64_t hash_coord(std::uint64_t seed, ChunkCoord chunk) noexcept
{
    std::uint64_t h = splitmix64(seed);
    h = splitmix64(h ^ static_cast<std::uint64_t>(static_cast<std::uint32_t>(chunk.cx)));
    h = splitmix64(h ^ static_cast<std::uint64_t>(static_cast<std::uint32_t>(chunk.cy)));
    return h;
}

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_RNG_HPP
