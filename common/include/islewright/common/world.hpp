#ifndef ISLEWRIGHT_COMMON_WORLD_HPP
#define ISLEWRIGHT_COMMON_WORLD_HPP

#include "islewright/common/biome.hpp"
#include "islewright/common/chunk.hpp"
#include "islewright/common/command.hpp"
#include "islewright/common/coords.hpp"
#include "islewright/common/tile.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <entt/entt.hpp>

namespace islewright::common {

// Hash for ChunkCoord: dense_map needs one and ChunkCoord has no default hash.
struct ChunkCoordHash
{
    std::size_t operator()(const ChunkCoord& c) const noexcept
    {
        return (static_cast<std::size_t>(static_cast<std::uint32_t>(c.x)) << 32) |
               static_cast<std::size_t>(static_cast<std::uint32_t>(c.y));
    }
};

class World
{
  public:
    explicit World(std::uint64_t seed);

    const entt::registry& Registry() const noexcept;

    template <auto Candidate, typename Type>
        requires std::invocable<decltype(Candidate), Type&, const entt::registry&, entt::entity>
    entt::connection ConnectChunkUpdates(Type& value)
    {
        return m_registry.on_update<Chunk>().connect<Candidate>(value);
    }

    void Apply(const Command& command);

    bool HasChunk(ChunkCoord coord) const noexcept;
    const Tile* TileAt(TileCoord coord) const noexcept;
    const BiomeType* BiomeAt(TileCoord coord) const noexcept;
    std::uint64_t Seed() const noexcept;

  private:
    entt::entity EmplaceChunk(Chunk chunk);
    entt::entity EnsureChunk(ChunkCoord coord);

    using ChunkIndex = entt::dense_map<ChunkCoord, entt::entity, ChunkCoordHash>;

    std::uint64_t m_seed;
    ChunkIndex m_index;
    entt::registry m_registry;
};

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_WORLD_HPP
