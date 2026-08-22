#ifndef ISLEWRIGHT_CHUNKRESPONSE_HPP
#define ISLEWRIGHT_CHUNKRESPONSE_HPP

#include "islewright.pb.h"
#include "islewright/common/chunk.hpp"

namespace islewright::chunkresponse {

void FillChunkResponse(const common::Chunk& chunk, protocol::ChunkResponse& response);

} // namespace islewright::chunkresponse

#endif // ISLEWRIGHT_CHUNKRESPONSE_HPP
