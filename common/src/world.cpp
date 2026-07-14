#include "islewright/common/world.hpp"

#include <cstddef>

namespace islewright::common {

World::World(std::uint64_t seed) : m_seed(seed) {}

entt::registry& World::Registry() noexcept
{
    return m_registry;
}

const entt::registry& World::Registry() const noexcept
{
    return m_registry;
}

entt::entity World::EmplaceChunk(Chunk chunk)
{
    const ChunkCoord coord = chunk.coord;
    if (auto it = m_index.find(coord); it != m_index.end()) {
        return it->second;
    }

    const entt::entity e = m_registry.create();
    m_registry.emplace<Chunk>(e, std::move(chunk));
    m_index.insert_or_assign(coord, e);
    return e;
}

bool World::HasChunk(ChunkCoord coord) const noexcept
{
    return m_index.contains(coord);
}

const Tile* World::TileAt(TileCoord tile) const noexcept
{
    const ChunkCoord chunkCoord = to_chunk(tile);
    const auto it = m_index.find(chunkCoord);
    if (it == m_index.end()) {
        return nullptr;
    }

    const entt::entity e = it->second;
    const Chunk* chunk = m_registry.try_get<Chunk>(e);
    if (!chunk) {
        return nullptr;
    }

    return &chunk->tiles[local_index(tile)];
}

const BiomeType* World::BiomeAt(TileCoord tile) const noexcept
{
    const Tile* t = TileAt(tile);
    return t ? &t->biome : nullptr;
}

std::uint64_t World::Seed() const noexcept
{
    return m_seed;
}

} // namespace islewright::common
