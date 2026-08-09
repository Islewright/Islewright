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

const Chunk* FindChunk(const World& world, ChunkCoord coord)
{
    auto view = world.Registry().view<const Chunk>();

    for (const entt::entity entity : view) {
        const Chunk& chunk = view.get<const Chunk>(entity);

        if (chunk.coord == coord) {
            return &chunk;
        }
    }

    return nullptr;
}

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

    // World forwards generation inputs and loading order does not affect results.
    constexpr ChunkCoord originCoord{.x = 0, .y = 0};
    constexpr ChunkCoord otherCoord{.x = 1, .y = 0};
    constexpr SetBiomeCommand originCommand{.coord = TileCoord{.x = 0, .y = 0},
                                            .biome = BiomeType::Mountain};
    constexpr SetBiomeCommand otherCommand{.coord = TileCoord{.x = CHUNK_WIDTH, .y = 0},
                                           .biome = BiomeType::Mountain};

    World first{42};
    first.Apply(originCommand);
    first.Apply(otherCommand);

    World second{42};
    second.Apply(otherCommand);
    second.Apply(originCommand);

    const Chunk* firstOrigin = FindChunk(first, originCoord);
    const Chunk* firstOther = FindChunk(first, otherCoord);
    const Chunk* secondOrigin = FindChunk(second, originCoord);
    const Chunk* secondOther = FindChunk(second, otherCoord);
    assert(firstOrigin != nullptr && firstOther != nullptr);
    assert(secondOrigin != nullptr && secondOther != nullptr);

    Chunk expectedOrigin{.coord = originCoord, .tiles = {}};
    Chunk expectedOther{.coord = otherCoord, .tiles = {}};

    generate_chunk(expectedOrigin, 42);
    generate_chunk(expectedOther, 42);

    expectedOrigin.tiles[0].biome = originCommand.biome;
    expectedOther.tiles[0].biome = otherCommand.biome;
    assert(firstOrigin->tiles == expectedOrigin.tiles);
    assert(firstOther->tiles == expectedOther.tiles);
    assert(secondOrigin->tiles == expectedOrigin.tiles);
    assert(secondOther->tiles == expectedOther.tiles);

    bool hasLand = false;

    for (const Tile& tile : firstOrigin->tiles) {
        hasLand |= tile.id == TileId::Grass || tile.id == TileId::Stone;
    }

    assert(hasLand);

    World different{43};
    different.Apply(originCommand);

    const Chunk* differentOrigin = FindChunk(different, originCoord);
    assert(differentOrigin != nullptr);

    Chunk expectedDifferentOrigin{.coord = originCoord, .tiles = {}};

    generate_chunk(expectedDifferentOrigin, 43);

    expectedDifferentOrigin.tiles[0].biome = originCommand.biome;
    assert(differentOrigin->tiles == expectedDifferentOrigin.tiles);
    assert(firstOrigin->tiles != differentOrigin->tiles);

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
