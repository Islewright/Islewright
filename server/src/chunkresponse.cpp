#include "chunkresponse.hpp"

#include <string>
#include <utility>

namespace islewright::chunkresponse {

void FillChunkResponse(const common::Chunk& chunk, protocol::ChunkResponse& response)
{
    std::string tileIds;
    std::string biomes;
    tileIds.reserve(chunk.tiles.size());
    biomes.reserve(chunk.tiles.size());

    for (const auto& tile : chunk.tiles) {
        tileIds.push_back(static_cast<char>(tile.id));
        biomes.push_back(static_cast<char>(tile.biome));
    }

    response.set_chunk_x(chunk.coord.x);
    response.set_chunk_y(chunk.coord.y);
    response.set_tile_ids(std::move(tileIds));
    response.set_biomes(std::move(biomes));
}

} // namespace islewright::chunkresponse
