#include "islewright/common/world.hpp"

#include <cassert>
#include <cstddef>
#include <limits>

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

    // generation is deterministic and independent of chunk load order
    World first{42};
    World second{42};

    const entt::entity firstOrigin = first.EnsureChunk(ChunkCoord{0, 0});
    const entt::entity firstOther = first.EnsureChunk(ChunkCoord{1, 0});
    const entt::entity secondOther = second.EnsureChunk(ChunkCoord{1, 0});
    const entt::entity secondOrigin = second.EnsureChunk(ChunkCoord{0, 0});

    assert(first.Registry().get<Chunk>(firstOrigin).tiles ==
           second.Registry().get<Chunk>(secondOrigin).tiles);
    assert(first.Registry().get<Chunk>(firstOther).tiles ==
           second.Registry().get<Chunk>(secondOther).tiles);
    assert(first.EnsureChunk(ChunkCoord{0, 0}) == firstOrigin);

    bool hasLand = false;

    for (const Tile& tile : first.Registry().get<Chunk>(firstOrigin).tiles) {
        hasLand |= tile.id == TileId::Grass || tile.id == TileId::Stone;
    }

    assert(hasLand);

    World different{43};
    const entt::entity differentOrigin = different.EnsureChunk(ChunkCoord{0, 0});

    assert(first.Registry().get<Chunk>(firstOrigin).tiles !=
           different.Registry().get<Chunk>(differentOrigin).tiles);

    const entt::entity distant = first.EnsureChunk(ChunkCoord{10, 10});

    for (const Tile& tile : first.Registry().get<Chunk>(distant).tiles) {
        assert((tile == Tile{TileId::Water, BiomeType::Ocean}));
    }

    const ChunkCoord extremeCoords[] = {
        ChunkCoord{std::numeric_limits<int>::min(), std::numeric_limits<int>::min()},
        ChunkCoord{std::numeric_limits<int>::max(), std::numeric_limits<int>::max()},
    };

    for (const ChunkCoord coord : extremeCoords) {
        const entt::entity extreme = first.EnsureChunk(coord);

        for (const Tile& tile : first.Registry().get<Chunk>(extreme).tiles) {
            assert((tile == Tile{TileId::Water, BiomeType::Ocean}));
        }
    }

    return 0;
}
