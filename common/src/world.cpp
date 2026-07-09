#include "islewright/common/world.hpp"

#include <cstddef>

namespace islewright::common {

World::World(std::uint64_t seed) : seed_(seed) {}

entt::registry& World::registry() noexcept
{
    return registry_;
}

const entt::registry& World::registry() const noexcept
{
    return registry_;
}

entt::entity World::emplace_chunk(Chunk chunk)
{
    const ChunkCoord coord = chunk.coord;
    if (auto it = index_.find(coord); it != index_.end()) {
        return it->second;
    }
    const entt::entity e = registry_.create();
    registry_.emplace<Chunk>(e, std::move(chunk));
    index_.insert_or_assign(coord, e);
    return e;
}

bool World::has_chunk(ChunkCoord coord) const noexcept
{
    return index_.contains(coord);
}

const Tile* World::tile_at(TileCoord tile) const noexcept
{
    const ChunkCoord chunk_coord = to_chunk(tile);
    const auto it = index_.find(chunk_coord);
    if (it == index_.end()) {
        return nullptr;
    }
    const entt::entity e = it->second;
    const Chunk& chunk = registry_.get<Chunk>(e);
    return &chunk.tiles[local_index(tile)];
}

const BiomeType* World::biome_at(TileCoord tile) const noexcept
{
    const Tile* t = tile_at(tile);
    return t ? &t->biome : nullptr;
}

std::uint64_t World::seed() const noexcept
{
    return seed_;
}

} // namespace islewright::common
