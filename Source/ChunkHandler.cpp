#include <ChunkHandler.h>


ChunkHandler::ChunkHandler() {

}

Chunk* ChunkHandler::GetChunk(int chunkX, int chunkY, int chunkZ) {
    auto chunk = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
    if (chunk != chunks.end()) {
        // Return the chunk if it exists
        return &(chunk->second);
    }
    else {
        // Return nullptr if not
        return nullptr;
    }

}

void ChunkHandler::AddChunk(int chunkX, int chunkY, int chunkZ) {
    chunks.insert(std::make_pair(std::tuple(chunkX, chunkY, chunkZ), Chunk(chunkX, chunkY, chunkZ)));
}


void ChunkHandler::Delete() {

}