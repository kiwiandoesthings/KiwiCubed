#include "ChunkHandler.h"
#include "World.h"


ChunkHandler::ChunkHandler(World& world) : world(world) {
}

void ChunkHandler::GenerateWorld() {
    world.GenerateWorld();
}

Chunk& ChunkHandler::GetChunk(int chunkX, int chunkY, int chunkZ) {
    auto chunk = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
    if (chunk != chunks.end()) {
        return chunk->second;
    }
    else {
        //std::cout << "[ChunkHandler / Debug] Chunk not found at {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
        Chunk& chunk = AddChunk(chunkX, chunkY, chunkZ);
        return chunk;
    }
}

Chunk& ChunkHandler::AddChunk(int chunkX, int chunkY, int chunkZ) {
    auto chunk = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
    if (chunk == chunks.end()) {
        chunks.insert(std::make_pair(std::tuple(chunkX, chunkY, chunkZ), Chunk(chunkX, chunkY, chunkZ)));
        chunk = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
        chunk->second.SetupRenderComponents();
        
        std::cout << "[ChunkHandler / Debug] Chunk added at {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
        world.totalChunks++;

        return chunk->second;
    }
    else {
        //std::cout << "[ChunkHandler / Debug] Chunk already found at {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
        return GetChunk(chunkX, chunkY, chunkZ);
    }
}

void ChunkHandler::DeleteChunk(int chunkX, int chunkY, int chunkZ) {
    GetChunk(chunkX, chunkY, chunkZ).Delete();
}

void ChunkHandler::GenerateChunk(int chunkX, int chunkY, int chunkZ, bool debug) {
    Chunk defaultChunk = Chunk(0, 0, 0);
    GetChunk(chunkX, chunkY, chunkZ).GenerateBlocks(world, defaultChunk, false, debug);
}

void ChunkHandler::MeshChunk(int chunkX, int chunkY, int chunkZ) {
    GetChunk(chunkX, chunkY, chunkZ).GenerateMesh(*this, false);
}

// Specifically uses the world's GenerateChunk() function that makes sure chunks mesh correctly
void ChunkHandler::SmartGenerateAndMeshChunk(int chunkX, int chunkY, int chunkZ) {
    Chunk chunk = GetChunk(chunkX, chunkY, chunkZ);
    world.GenerateChunk(chunkX, chunkY, chunkZ, chunk, false, chunk);
}

// Meshes a chunk fully no matter what, even if it needs to alloate / generate surrounding chunks too
void ChunkHandler::ForceGenerateAndMeshChunk(int chunkX, int chunkY, int chunkZ) {
    AddChunk(chunkX, chunkY, chunkZ);
    AddChunk(chunkX - 1, chunkY, chunkZ);
    AddChunk(chunkX, chunkY - 1, chunkZ);
    AddChunk(chunkX, chunkY, chunkZ - 1);
    AddChunk(chunkX + 1, chunkY, chunkZ);
    AddChunk(chunkX, chunkY + 1, chunkZ);
    AddChunk(chunkX, chunkY, chunkZ + 1);
    GenerateChunk(chunkX, chunkY, chunkZ, true);
    GenerateChunk(chunkX - 1, chunkY, chunkZ, true);
    GenerateChunk(chunkX, chunkY - 1, chunkZ, true);
    GenerateChunk(chunkX, chunkY, chunkZ - 1, true);
    GenerateChunk(chunkX + 1, chunkY, chunkZ, true);
    GenerateChunk(chunkX, chunkY + 1, chunkZ, true);
    GenerateChunk(chunkX, chunkY, chunkZ + 1, true);
    MeshChunk(chunkX, chunkY, chunkZ);
}

void ChunkHandler::RemeshChunk(int chunkX, int chunkY, int chunkZ, bool updateNeighbors) {
    GetChunk(chunkX, chunkY, chunkZ).GenerateMesh(*this, true);

    if (updateNeighbors) {
        GetChunk(chunkX + 1, chunkY, chunkZ).GenerateMesh(*this, true);
        GetChunk(chunkX - 1, chunkY, chunkZ).GenerateMesh(*this, true);
        GetChunk(chunkX, chunkY + 1, chunkZ).GenerateMesh(*this, true);
        GetChunk(chunkX, chunkY - 1, chunkZ).GenerateMesh(*this, true);
        GetChunk(chunkX, chunkY, chunkZ + 1).GenerateMesh(*this, true);
        GetChunk(chunkX, chunkY, chunkZ - 1).GenerateMesh(*this, true);
    }
}

void ChunkHandler::AddBlock(int chunkX, int chunkY, int chunkZ, int blockX, int blockY, int blockZ, int type) {
    Chunk& chunk = GetChunk(chunkX, chunkY, chunkZ);
    Block& block = chunk.blocks[blockX][blockY][blockZ];
    if (block.GetType() == 0 && type != 0) {
        chunk.SetTotalBlocks(chunk.GetTotalBlocks() + 1);
    }
    else if (block.GetType() != 0 && type == 0) {
        chunk.SetTotalBlocks(chunk.GetTotalBlocks() - 1);
    }
    block.SetType(type);
}

void ChunkHandler::RemoveBlock(int chunkX, int chunkY, int chunkZ, int blockX, int blockY, int blockZ) {
    AddBlock(chunkX, chunkY, chunkZ, blockX, blockY, blockZ, 0);
}


void ChunkHandler::Delete() {
    for (auto it = chunks.begin(); it != chunks.end(); ++it) {
        const auto& key = it->first;
        auto& chunk = it->second;
        chunk.Delete();
        world.totalChunks--;
    }
}