#include "islewright/common/worldgen.hpp"

#include "islewright/common/rng.hpp"

#include <cstddef>
#include <cstdint>
#include <random>

namespace islewright::common {
namespace {

constexpr std::int64_t ISLAND_RADIUS = 48;
constexpr std::int64_t SHORE_JITTER = 4;
constexpr std::int64_t MAX_RADIUS = ISLAND_RADIUS + SHORE_JITTER;

} // namespace

void generate_chunk(Chunk& chunk, std::uint64_t seed)
{
    std::mt19937_64 rng{hash_coord(seed, chunk.coord)};

    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            const std::int64_t worldX = static_cast<std::int64_t>(chunk.coord.x) * CHUNK_WIDTH + x;
            const std::int64_t worldY = static_cast<std::int64_t>(chunk.coord.y) * CHUNK_HEIGHT + y;
            const std::int64_t radius =
                ISLAND_RADIUS + static_cast<std::int64_t>(rng() % 9) - SHORE_JITTER;

            Tile tile{TileId::Water, BiomeType::Ocean};

            if (worldX >= -MAX_RADIUS && worldX <= MAX_RADIUS && worldY >= -MAX_RADIUS &&
                worldY <= MAX_RADIUS && worldX * worldX + worldY * worldY <= radius * radius) {
                const std::uint64_t roll = rng() % 100;
                tile = roll < 70   ? Tile{TileId::Grass, BiomeType::Plain}
                       : roll < 90 ? Tile{TileId::Grass, BiomeType::Forest}
                                   : Tile{TileId::Stone, BiomeType::Mountain};
            }

            chunk.tiles[static_cast<std::size_t>(y * CHUNK_WIDTH + x)] = tile;
        }
    }
}

} // namespace islewright::common
