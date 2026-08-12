#ifndef ISLEWRIGHT_COMMON_CONSTANTS_HPP
#define ISLEWRIGHT_COMMON_CONSTANTS_HPP

namespace islewright::common {

// Dimensions of a single chunk in tiles. Chunk-local indices span
// [0, CHUNK_WIDTH) x [0, CHUNK_HEIGHT). The tile <-> chunk conversions in coords.hpp
// depend on these; chunk.hpp reuses them.
inline constexpr int CHUNK_WIDTH = 32;
inline constexpr int CHUNK_HEIGHT = 32;

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_CONSTANTS_HPP
