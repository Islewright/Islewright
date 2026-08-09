#include "islewright/common/command.hpp"
#include "islewright/common/world.hpp"
#include "islewright/common/worldgen.hpp"

#include <cassert>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

using namespace islewright::common;

static_assert(std::is_same_v<decltype(std::declval<World&>().Registry()), const entt::registry&>);

namespace {
struct ChunkUpdateCounter
{
    void OnUpdate(const entt::registry&, entt::entity)
    {
        ++count;
    }

    int count{};
};

struct MutableChunkUpdateCounter
{
    void OnUpdate(entt::registry&, entt::entity)
    {
        // Do nothing
    }
};

} // namespace

template <typename WorldType, typename Receiver>
concept CanConnectChunkUpdates = requires(WorldType& world, Receiver& receiver) {
    world.template ConnectChunkUpdates<&Receiver::OnUpdate>(receiver);
};

template <typename WorldType>
concept CanEmplaceChunk = requires(WorldType& world, Chunk chunk) { world.EmplaceChunk(chunk); };

template <typename WorldType>
concept CanEnsureChunk = requires(WorldType& world, ChunkCoord coord) { world.EnsureChunk(coord); };

static_assert(!CanConnectChunkUpdates<World, MutableChunkUpdateCounter>);
static_assert(!CanEmplaceChunk<World>);
static_assert(!CanEnsureChunk<World>);

int main()
{
    World world{42};
    assert(world.Seed() == 42);
    assert(world.TileAt(TileCoord{0, 0}) == nullptr);
    assert(world.BiomeAt(TileCoord{0, 0}) == nullptr);
    assert(!world.HasChunk(ChunkCoord{0, 0}));

    const World& readOnlyWorld = world;
    assert(&readOnlyWorld.Registry() == &world.Registry());

    // Generation is deterministic and independent of call order.
    Chunk firstOrigin{.coord = ChunkCoord{.x = 0, .y = 0}, .tiles = {}};
    Chunk firstOther{.coord = ChunkCoord{.x = 1, .y = 0}, .tiles = {}};
    Chunk secondOrigin{.coord = ChunkCoord{.x = 0, .y = 0}, .tiles = {}};
    Chunk secondOther{.coord = ChunkCoord{.x = 1, .y = 0}, .tiles = {}};

    generate_chunk(firstOrigin, 42);
    generate_chunk(firstOther, 42);
    generate_chunk(secondOther, 42);
    generate_chunk(secondOrigin, 42);

    assert(firstOrigin.tiles == secondOrigin.tiles);
    assert(firstOther.tiles == secondOther.tiles);

    bool hasLand = false;

    for (const Tile& tile : firstOrigin.tiles) {
        hasLand |= tile.id == TileId::Grass || tile.id == TileId::Stone;
    }

    assert(hasLand);

    Chunk differentOrigin{.coord = firstOrigin.coord, .tiles = {}};

    generate_chunk(differentOrigin, 43);

    assert(firstOrigin.tiles != differentOrigin.tiles);

    Chunk distant{.coord = ChunkCoord{.x = 10, .y = 10}};

    generate_chunk(distant, 42);

    for (const Tile& tile : distant.tiles) {
        assert((tile == Tile{TileId::Water, BiomeType::Ocean}));
    }

    constexpr ChunkCoord extremeCoords[] = {
        ChunkCoord{.x = std::numeric_limits<int>::min(), .y = std::numeric_limits<int>::min()},
        ChunkCoord{.x = std::numeric_limits<int>::max(), .y = std::numeric_limits<int>::max()},
    };

    for (const ChunkCoord coord : extremeCoords) {
        Chunk extreme{.coord = coord, .tiles = {}};

        generate_chunk(extreme, 42);

        for (const Tile& tile : extreme.tiles) {
            assert((tile == Tile{TileId::Water, BiomeType::Ocean}));
        }
    }

    World commands{42};
    ChunkUpdateCounter updates;
    entt::scoped_connection connection{
        commands.ConnectChunkUpdates<&ChunkUpdateCounter::OnUpdate>(updates)};
    constexpr TileCoord target{.x = -161, .y = -161};

    commands.Apply(PlaceTileCommand{.coord = target, .tile = TileId::Stone});
    assert(commands.HasChunk(to_chunk(target)));

    const Tile* tile = commands.TileAt(target);
    assert(tile != nullptr && tile->id == TileId::Stone);
    assert(tile->biome == BiomeType::Ocean);
    assert(updates.count == 1);

    commands.Apply(SetBiomeCommand{.coord = target, .biome = BiomeType::Mountain});

    tile = commands.TileAt(target);
    assert(tile != nullptr && tile->id == TileId::Stone);
    assert(tile->biome == BiomeType::Mountain);
    assert(updates.count == 2);

    return 0;
}
