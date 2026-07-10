#include "islewright/common/world.hpp"

#include <cassert>
#include <cstddef>

using namespace islewright::common;

int main()
{
    World w{42};
    assert(w.seed() == 42);

    // empty world: nothing loaded
    assert(w.tile_at(TileCoord{0, 0}) == nullptr);
    assert(w.biome_at(TileCoord{0, 0}) == nullptr);
    assert(!w.has_chunk(ChunkCoord{0, 0}));

    // insert a chunk and read a tile back
    Chunk c{};
    c.coord = ChunkCoord{0, 0};
    c.tiles[static_cast<std::size_t>(local_index(TileCoord{5, 3}))] =
        Tile{TileId::Water, BiomeType::Ocean};
    const entt::entity e = w.emplace_chunk(c);

    assert(w.has_chunk(ChunkCoord{0, 0}));
    assert(w.registry().all_of<Chunk>(e));
    const Tile* t = w.tile_at(TileCoord{5, 3});
    assert(t != nullptr && t->id == TileId::Water);
    const BiomeType* b = w.biome_at(TileCoord{5, 3});
    assert(b != nullptr && *b == BiomeType::Ocean);

    // negative coordinates
    Chunk cn{};
    cn.coord = ChunkCoord{-1, -1};
    cn.tiles[static_cast<std::size_t>(local_index(TileCoord{-1, -1}))] =
        Tile{TileId::Stone, BiomeType::Mountain};
    w.emplace_chunk(cn);
    const Tile* tn = w.tile_at(TileCoord{-1, -1});
    assert(tn != nullptr && tn->id == TileId::Stone);

    // const correctness
    const World& cw = w;
    assert(&cw.registry() == &w.registry());

    // duplicate coord: emplace_chunk returns the existing entity
    Chunk dup{};
    dup.coord = ChunkCoord{0, 0};
    assert(w.emplace_chunk(dup) == e);

    return 0;
}
