#include "ChunkHandler.h"
#include "World.h"


ChunkHandler::ChunkHandler(World& world) : world(world) {
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

void ChunkHandler::GenerateAndMeshChunk(int chunkX, int chunkY, int chunkZ) {
    Chunk chunk = GetChunk(chunkX, chunkY, chunkZ);
    world.GenerateChunk(chunkX, chunkY, chunkZ, chunk, false, chunk);
}


void ChunkHandler::Delete() {

}