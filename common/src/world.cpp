#include "islewright/common/world.hpp"

#include "islewright/common/worldgen.hpp"

#include <cstddef>
#include <type_traits>
#include <variant>

namespace islewright::common {

World::World(std::uint64_t seed) : m_seed(seed) {}

const entt::registry& World::Registry() const noexcept
{
    return m_registry;
}

void World::Apply(const Command& command)
{
    std::visit(
        [this]<typename T>(const T& value) {
            const entt::entity entity = EnsureChunk(to_chunk(value.coord));

            m_registry.patch<Chunk>(entity, [&value](Chunk& chunk) {
                Tile& tile = chunk.tiles[static_cast<std::size_t>(local_index(value.coord))];

                if constexpr (std::is_same_v<std::decay_t<T>, PlaceTileCommand>) {
                    tile.id = value.tile;
                } else {
                    tile.biome = value.biome;
                }
            });
        },
        command);
}

entt::entity World::EmplaceChunk(Chunk chunk)
{
    const ChunkCoord coord = chunk.coord;

    if (auto it = m_index.find(coord); it != m_index.end()) {
        return it->second;
    }

    const entt::entity entity = m_registry.create();

    m_registry.emplace<Chunk>(entity, chunk);
    m_index.insert_or_assign(coord, entity);

    return entity;
}

entt::entity World::EnsureChunk(ChunkCoord coord)
{
    if (const auto it = m_index.find(coord); it != m_index.end()) {
        return it->second;
    }

    Chunk chunk{};
    chunk.coord = coord;

    generate_chunk(chunk, m_seed);

    return EmplaceChunk(chunk);
}

bool World::HasChunk(ChunkCoord coord) const noexcept
{
    return m_index.contains(coord);
}

const Tile* World::TileAt(TileCoord coord) const noexcept
{
    const ChunkCoord chunkCoord = to_chunk(coord);
    const auto it = m_index.find(chunkCoord);

    if (it == m_index.end()) {
        return nullptr;
    }

    const entt::entity entity = it->second;
    const Chunk* chunk = m_registry.try_get<Chunk>(entity);

    if (!chunk) {
        return nullptr;
    }

    return &chunk->tiles[local_index(coord)];
}

const BiomeType* World::BiomeAt(TileCoord coord) const noexcept
{
    const Tile* tile = TileAt(coord);
    return tile ? &tile->biome : nullptr;
}

std::uint64_t World::Seed() const noexcept
{
    return m_seed;
}

} // namespace islewright::common
