#include <ChunkHandler.h>


ChunkHandler::ChunkHandler() {

}

Chunk* ChunkHandler::GetChunk(int chunkX, int chunkY, int chunkZ) {
    auto it = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
    if (it != chunks.end()) {
        // Return the chunk if it exists
        return &(it->second);
    }
    else {
        // Return nullptr if not
        return nullptr;
    }

}


void ChunkHandler::Delete() {

}