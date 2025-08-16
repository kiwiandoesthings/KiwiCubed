#include "ChunkHandler.h"
#include "World.h"

void ChunkHandler::GenerateWorld() {
    world.GenerateWorld();
}

Chunk& ChunkHandler::GetChunk(int chunkX, int chunkY, int chunkZ, bool addIfNotFound) {
    OVERRIDE_LOG_NAME("ChunkHandler");
    std::lock_guard<std::mutex> lock(ChunkMutex);
    auto chunk = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
    if (chunk != chunks.end()) {
        return chunk->second;
    }
    else {
        //WARN("Chunk not found at {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}");
        if (addIfNotFound) {
            Chunk& chunk = AddChunk(chunkX, chunkY, chunkZ);
            return chunk;
        } else {
            return defaultChunk;
        }
    }
}

bool ChunkHandler::GetChunkExists(int chunkX, int chunkY, int chunkZ) {
    auto chunk = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
    if (chunk != chunks.end()) {
        return true;
    }
    else {
        return false;
    }
}

Chunk& ChunkHandler::AddChunk(int chunkX, int chunkY, int chunkZ) {
    OVERRIDE_LOG_NAME("ChunkHandler");
    auto chunk = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
    if (chunk == chunks.end()) {
        chunks.insert(std::make_pair(std::tuple<int, int, int>(chunkX, chunkY, chunkZ), Chunk(chunkX, chunkY, chunkZ)));
        chunk = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
        chunk->second.SetPosition(chunkX, chunkY, chunkZ);
        chunk->second.AllocateChunk();
        chunk->second.id = world.totalChunks;

        world.totalChunks++;

        return chunk->second;
    }
    else {
        //INFO("Chunk already found at {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}");
        return GetChunk(chunkX, chunkY, chunkZ, false);
    }
}

void ChunkHandler::GenerateChunk(int chunkX, int chunkY, int chunkZ, Chunk callerChunk, bool updateCallerChunk, bool debug) {
    GetChunk(chunkX, chunkY, chunkZ, false).GenerateBlocks(world, callerChunk, updateCallerChunk, debug);
}

bool ChunkHandler::MeshChunk(int chunkX, int chunkY, int chunkZ) {
    return GetChunk(chunkX, chunkY, chunkZ, false).GenerateMesh(*this, false);
}

// Specifically uses the world's GenerateChunk() function that makes sure chunks mesh correctly
void ChunkHandler::SmartGenerateAndMeshChunk(int chunkX, int chunkY, int chunkZ) {
    Chunk chunk = GetChunk(chunkX, chunkY, chunkZ, false);
    world.GenerateChunk(chunkX, chunkY, chunkZ, chunk, false, chunk);
}

// Meshes a chunk fully no matter what, even if it needs to allocate / generate surrounding chunks too
void ChunkHandler::ForceGenerateAndMeshChunk(int chunkX, int chunkY, int chunkZ) {
    AddChunk(chunkX, chunkY, chunkZ);
    AddChunk(chunkX - 1, chunkY, chunkZ);
    AddChunk(chunkX, chunkY - 1, chunkZ);
    AddChunk(chunkX, chunkY, chunkZ - 1);
    AddChunk(chunkX + 1, chunkY, chunkZ);
    AddChunk(chunkX, chunkY + 1, chunkZ);
    AddChunk(chunkX, chunkY, chunkZ + 1);
    GenerateChunk(chunkX, chunkY, chunkZ, defaultChunk, false, true);
    GenerateChunk(chunkX - 1, chunkY, chunkZ, defaultChunk, false, true);
    GenerateChunk(chunkX, chunkY - 1, chunkZ, defaultChunk, false, true);
    GenerateChunk(chunkX, chunkY, chunkZ - 1, defaultChunk, false, true);
    GenerateChunk(chunkX + 1, chunkY, chunkZ, defaultChunk, false, true);
    GenerateChunk(chunkX, chunkY + 1, chunkZ, defaultChunk, false, true);
    GenerateChunk(chunkX, chunkY, chunkZ + 1, defaultChunk, false, true);
    MeshChunk(chunkX, chunkY, chunkZ);
}

void ChunkHandler::RemeshChunk(int chunkX, int chunkY, int chunkZ, bool updateNeighbors) {
    Chunk& chunk = GetChunk(chunkX, chunkY, chunkZ, false);
    if (chunk.generationStatus < 2) {
        return;
    }

    chunk.GenerateMesh(*this, true);

    if (updateNeighbors) {
        GetChunk(chunkX + 1, chunkY, chunkZ, false).GenerateMesh(*this, true);
        GetChunk(chunkX - 1, chunkY, chunkZ, false).GenerateMesh(*this, true);
        GetChunk(chunkX, chunkY + 1, chunkZ, false).GenerateMesh(*this, true);
        GetChunk(chunkX, chunkY - 1, chunkZ, false).GenerateMesh(*this, true);
        GetChunk(chunkX, chunkY, chunkZ + 1, false).GenerateMesh(*this, true);
        GetChunk(chunkX, chunkY, chunkZ - 1, false).GenerateMesh(*this, true);
    }
}

void ChunkHandler::AddBlock(int chunkX, int chunkY, int chunkZ, int blockX, int blockY, int blockZ, unsigned short newBlockID) {
    Chunk& chunk = GetChunk(chunkX, chunkY, chunkZ, false);
    Block& block = chunk.blocks[blockX][blockY][blockZ];
    if (block.IsAir() ^ (newBlockID == 0)) {
        int currentBlocks = chunk.GetTotalBlocks();
        if (newBlockID == 0) {
            chunk.SetTotalBlocks(currentBlocks - 1);
        }
        else {
            chunk.SetTotalBlocks(currentBlocks + 1);
        }
    }
    block.SetBlockID(newBlockID);
}

void ChunkHandler::RemoveBlock(int chunkX, int chunkY, int chunkZ, int blockX, int blockY, int blockZ) {
    AddBlock(chunkX, chunkY, chunkZ, blockX, blockY, blockZ, 0);
}

void ChunkHandler::Delete() {
    for (auto it = chunks.begin(); it != chunks.end(); ++it) {
        auto& chunk = it->second;
        chunk.Delete();
        world.totalChunks--;
    }

    chunks.clear();
    world.totalMemoryUsage = 0;
}