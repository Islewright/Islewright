#ifndef ISLEWRIGHT_COMMON_WORLDGEN_HPP
#define ISLEWRIGHT_COMMON_WORLDGEN_HPP

#include "islewright/common/chunk.hpp"

#include <cstdint>

namespace islewright::common {

void generate_chunk(Chunk& chunk, std::uint64_t seed);

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_WORLDGEN_HPP
