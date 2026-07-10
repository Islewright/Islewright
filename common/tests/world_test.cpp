#include "islewright/common/world.hpp"

#include <cassert>
#include <cstddef>

using namespace islewright::common;

int main()
{
    World w{42};
    assert(w.Seed() == 42);

    // empty world: nothing loaded
    assert(w.TileAt(TileCoord{0, 0}) == nullptr);
    assert(w.BiomeAt(TileCoord{0, 0}) == nullptr);
    assert(!w.HasChunk(ChunkCoord{0, 0}));

    // insert a chunk and read a tile back
    Chunk c{};
    c.coord = ChunkCoord{0, 0};
    c.tiles[static_cast<std::size_t>(local_index(TileCoord{5, 3}))] =
        Tile{TileId::Water, BiomeType::Ocean};
    const entt::entity e = w.EmplaceChunk(c);

    assert(w.HasChunk(ChunkCoord{0, 0}));
    assert(w.Registry().all_of<Chunk>(e));
    const Tile* t = w.TileAt(TileCoord{5, 3});
    assert(t != nullptr && t->id == TileId::Water);
    const BiomeType* b = w.BiomeAt(TileCoord{5, 3});
    assert(b != nullptr && *b == BiomeType::Ocean);

    // negative coordinates
    Chunk cn{};
    cn.coord = ChunkCoord{-1, -1};
    cn.tiles[static_cast<std::size_t>(local_index(TileCoord{-1, -1}))] =
        Tile{TileId::Stone, BiomeType::Mountain};
    w.EmplaceChunk(cn);
    const Tile* tn = w.TileAt(TileCoord{-1, -1});
    assert(tn != nullptr && tn->id == TileId::Stone);

    // const correctness
    const World& cw = w;
    assert(&cw.Registry() == &w.Registry());

    // duplicate coord: EmplaceChunk returns the existing entity
    Chunk dup{};
    dup.coord = ChunkCoord{0, 0};
    assert(w.EmplaceChunk(dup) == e);

    return 0;
}
