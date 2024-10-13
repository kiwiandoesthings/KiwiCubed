#include "ChunkHandler.h"


ChunkHandler::ChunkHandler() {

}

Chunk& ChunkHandler::GetChunk(int chunkX, int chunkY, int chunkZ) {
    static Chunk defaultChunk = Chunk(0, 0, 0);
    auto chunk = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
    if (chunk != chunks.end()) {
        return chunk->second;
    }
    else {
        //std::cout << "[Debug]: Chunk not found at {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
        return defaultChunk;
    }
}

void ChunkHandler::AddChunk(int chunkX, int chunkY, int chunkZ) {
    chunks.insert(std::make_pair(std::tuple(chunkX, chunkY, chunkZ), Chunk(chunkX, chunkY, chunkZ)));
}


void ChunkHandler::Delete() {

}