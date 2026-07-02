#include "islewright/common/world.hpp"

#include <cstddef>
#include <optional>
#include <utility>

namespace islewright::common {

World::World(std::uint64_t seed) : seed_{seed}, index_{}, registry_{}
{
    registry_.on_construct<Chunk>().connect<&World::on_chunk_constructed>(*this);
    registry_.on_destroy<Chunk>().connect<&World::on_chunk_destroyed>(*this);
}

World::~World()
{
    // Drop the listeners before the registry so none can fire on a dead World.
    registry_.on_construct<Chunk>().disconnect<&World::on_chunk_constructed>(*this);
    registry_.on_destroy<Chunk>().disconnect<&World::on_chunk_destroyed>(*this);
}

void World::on_chunk_constructed(entt::registry& reg, entt::entity e)
{
    index_.insert_or_assign(reg.get<Chunk>(e).coord, e);
}

void World::on_chunk_destroyed(entt::registry& reg, entt::entity e)
{
    const ChunkCoord coord = reg.get<Chunk>(e).coord;
    // Clear the mapping only if it still points at this entity.
    if (const auto it = index_.find(coord); it != index_.end() && it->second == e) {
        index_.erase(it);
    }
}

entt::entity World::emplace_chunk(Chunk chunk)
{
    const entt::entity e = registry_.create();
    registry_.emplace<Chunk>(e, std::move(chunk)); // fires on_construct -> indexed
    return e;
}

const Tile* World::tile_at(TileCoord tile) const noexcept
{
    const ChunkCoord cc = to_chunk(tile);
    const auto it = index_.find(cc);
    if (it == index_.end()) {
        return nullptr;
    }
    // try_get, not get: a desync degrades to nullptr instead of UB.
    const Chunk* chunk = registry_.try_get<Chunk>(it->second);
    if (chunk == nullptr) {
        return nullptr;
    }
    return &chunk->tiles[static_cast<std::size_t>(local_index(tile))];
}

std::optional<BiomeType> World::biome_at(TileCoord tile) const noexcept
{
    if (const Tile* t = tile_at(tile)) {
        return t->biome;
    }
    return std::nullopt;
}

} // namespace islewright::common
