#ifndef ISLEWRIGHT_COMMON_WORLD_HPP
#define ISLEWRIGHT_COMMON_WORLD_HPP

#include "islewright/common/biome.hpp"
#include "islewright/common/chunk.hpp"
#include "islewright/common/coords.hpp"
#include "islewright/common/tile.hpp"

#include <cstddef>
#include <cstdint>
#include <entt/entt.hpp>

namespace islewright::common {

// Hash for ChunkCoord: dense_map needs one and ChunkCoord has no default hash.
struct ChunkCoordHash
{
    std::size_t operator()(const ChunkCoord& c) const noexcept
    {
        return (static_cast<std::size_t>(static_cast<std::uint32_t>(c.cx)) << 32) |
               static_cast<std::size_t>(static_cast<std::uint32_t>(c.cy));
    }
};

class World
{
  public:
    explicit World(std::uint64_t seed);

    entt::registry& registry() noexcept;

    entt::entity emplace_chunk(Chunk chunk);

    bool has_chunk(ChunkCoord coord) const noexcept;
    const Tile* tile_at(TileCoord tile) const noexcept;
    const BiomeType* biome_at(TileCoord tile) const noexcept;
    std::uint64_t seed() const noexcept;

  private:
    using ChunkIndex = entt::dense_map<ChunkCoord, entt::entity, ChunkCoordHash>;

    std::uint64_t seed_;
    ChunkIndex index_;
    entt::registry registry_;
};

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_WORLD_HPP
