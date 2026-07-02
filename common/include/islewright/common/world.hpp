#ifndef ISLEWRIGHT_COMMON_WORLD_HPP
#define ISLEWRIGHT_COMMON_WORLD_HPP

#include "islewright/common/biome.hpp"
#include "islewright/common/chunk.hpp"
#include "islewright/common/coords.hpp"
#include "islewright/common/rng.hpp"
#include "islewright/common/tile.hpp"

#include <cstddef>
#include <cstdint>
#include <entt/entt.hpp>
#include <optional>

namespace islewright::common {

// Hash for ChunkCoord, used only to bucket index entries (unrelated to world
// determinism). Cast each axis through uint32_t before packing so negative
// coordinates do not collide, then splitmix64 for spread.
struct ChunkCoordHash
{
    [[nodiscard]] std::size_t operator()(const ChunkCoord& c) const noexcept
    {
        const std::uint64_t k =
            (static_cast<std::uint64_t>(static_cast<std::uint32_t>(c.cx)) << 32) |
            static_cast<std::uint64_t>(static_cast<std::uint32_t>(c.cy));
        return static_cast<std::size_t>(splitmix64(k));
    }
};

// Owns the authoritative registry for one world, plus its seed and a
// ChunkCoord -> entity index kept in sync by the on_construct/on_destroy<Chunk>
// signals. Non-copyable and non-movable: the listeners hold a raw pointer to
// *this that a move would not rebind. Hold one by reference or via unique_ptr.
class World
{
  public:
    explicit World(std::uint64_t seed);
    ~World();

    World(const World&) = delete;
    World& operator=(const World&) = delete;
    World(World&&) = delete;
    World& operator=(World&&) = delete;

    // The underlying registry. Adding/removing a Chunk through it still keeps
    // the spatial index in sync (same signals as emplace_chunk).
    [[nodiscard]] entt::registry& registry() noexcept
    {
        return registry_;
    }

    [[nodiscard]] const entt::registry& registry() const noexcept
    {
        return registry_;
    }

    // Creates an entity holding the given chunk (indexed by its coord) and
    // returns it; no generation happens here. A chunk's coord is immutable
    // afterward -- to move a chunk, destroy the entity and re-emplace. Duplicate
    // coords are last-wins in the index (the older entity is orphaned); keeping
    // one chunk per coord is the caller's responsibility.
    entt::entity emplace_chunk(Chunk chunk);

    // True if a chunk is indexed at this chunk coordinate.
    [[nodiscard]] bool has_chunk(ChunkCoord coord) const noexcept
    {
        return index_.contains(coord);
    }

    // Tile at a world coordinate, or nullptr if its chunk is not loaded. The
    // pointer is invalidated by any later chunk removal; copy the Tile to keep it.
    [[nodiscard]] const Tile* tile_at(TileCoord tile) const noexcept;

    // Biome at a world coordinate, or nullopt if its chunk is not loaded, so
    // "absent" stays distinct from a genuinely-Ocean tile.
    [[nodiscard]] std::optional<BiomeType> biome_at(TileCoord tile) const noexcept;

    // World seed, fixed at construction; never derived from entity ids or order.
    [[nodiscard]] std::uint64_t seed() const noexcept
    {
        return seed_;
    }

  private:
    // The only writers of index_.
    void on_chunk_constructed(entt::registry& reg, entt::entity e);
    void on_chunk_destroyed(entt::registry& reg, entt::entity e);

    // registry_ is declared last so it is destroyed first, while index_ is still
    // alive.
    std::uint64_t seed_;
    entt::dense_map<ChunkCoord, entt::entity, ChunkCoordHash> index_;
    entt::registry registry_;
};

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_WORLD_HPP
