#include "ChunkHandler.h"
#include "World.h"


ChunkHandler::ChunkHandler(World& world) : world(world) {
    defaultChunk = std::make_unique<Chunk>(0, 0, 0, *this);
}

void ChunkHandler::GenerateWorld() {
    world.GenerateWorld();
}

void ChunkHandler::CleanChunks() {
    for (auto iterator = chunks.begin(); iterator != chunks.end();) {
        if (iterator->second->shouldDelete) {
            iterator->second->Delete();
            iterator = chunks.erase(iterator);
        } else {
            iterator++;
        }
    }
}

Chunk* ChunkHandler::GetChunk(int chunkX, int chunkY, int chunkZ, bool addIfNotFound) {
    if (chunkX == 0 && chunkY == 0 && chunkZ == 0) {
		//owo;
		int x = 2;
	}
	
	OVERRIDE_LOG_NAME("ChunkHandler");
    std::lock_guard<std::mutex> lock(ChunkMutex);
    auto chunk = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
    if (chunk != chunks.end()) {
        return chunk->second.get();
    }
    else {
        //DEBUG("Chunk not found at {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}", Globals::GetInstance().debugMode);
        if (addIfNotFound) {
            Chunk* chunk = AddChunkUnlocked(chunkX, chunkY, chunkZ);
            return chunk;
        } else {
            return defaultChunk.get();
        }
    }
}

bool ChunkHandler::GetChunkExists(int chunkX, int chunkY, int chunkZ) {
    std::lock_guard<std::mutex> lock(ChunkMutex);
    auto chunk = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
    if (chunk != chunks.end()) {
        return true;
    }
    else {
        return false;
    }
}

Chunk* ChunkHandler::AddChunk(int chunkX, int chunkY, int chunkZ) {
    OVERRIDE_LOG_NAME("ChunkHandler");
    std::lock_guard<std::mutex> lock(ChunkMutex);
    return AddChunkUnlocked(chunkX, chunkY, chunkZ);
}

void ChunkHandler::GenerateChunk(int chunkX, int chunkY, int chunkZ, Chunk* callerChunk, bool updateCallerChunk, bool debug) {
    GetChunk(chunkX, chunkY, chunkZ, false)->GenerateBlocks(world, callerChunk, updateCallerChunk, debug);
}

bool ChunkHandler::MeshChunk(int chunkX, int chunkY, int chunkZ) {
    Chunk* chunk = GetChunk(chunkX, chunkY, chunkZ, false);
    if (chunk == nullptr) {
        WARN("Tried to mesh chunk that didn't exist {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}, aborting");
        return false;
    }
    
    return chunk->GenerateMesh(false);
}

// Specifically uses the world's GenerateChunk() function that makes sure chunks mesh correctly
void ChunkHandler::SmartGenerateAndMeshChunk(int chunkX, int chunkY, int chunkZ) {
    Chunk* chunk = GetChunk(chunkX, chunkY, chunkZ, false);
    world.GenerateChunk(chunkX, chunkY, chunkZ, chunk, false, nullptr);
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
    GenerateChunk(chunkX, chunkY, chunkZ, nullptr, false, true);
    GenerateChunk(chunkX - 1, chunkY, chunkZ, nullptr, false, true);
    GenerateChunk(chunkX, chunkY - 1, chunkZ, nullptr, false, true);
    GenerateChunk(chunkX, chunkY, chunkZ - 1, nullptr, false, true);
    GenerateChunk(chunkX + 1, chunkY, chunkZ, nullptr, false, true);
    GenerateChunk(chunkX, chunkY + 1, chunkZ, nullptr, false, true);
    GenerateChunk(chunkX, chunkY, chunkZ + 1, nullptr, false, true);
    MeshChunk(chunkX, chunkY, chunkZ);
}

void ChunkHandler::RemeshChunk(int chunkX, int chunkY, int chunkZ, bool updateNeighbors) {
    Chunk* chunk = GetChunk(chunkX, chunkY, chunkZ, false);
    if (chunk->GetGenerationStatus() < 2) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(ChunkMutex);
        chunk->GenerateMesh(true);

        if (updateNeighbors) {
            GetChunkUnlocked(chunkX + 1, chunkY, chunkZ, false)->GenerateMesh(true);
            GetChunkUnlocked(chunkX - 1, chunkY, chunkZ, false)->GenerateMesh(true);
            GetChunkUnlocked(chunkX, chunkY + 1, chunkZ, false)->GenerateMesh(true);
            GetChunkUnlocked(chunkX, chunkY - 1, chunkZ, false)->GenerateMesh(true);
            GetChunkUnlocked(chunkX, chunkY, chunkZ + 1, false)->GenerateMesh(true);
            GetChunkUnlocked(chunkX, chunkY, chunkZ - 1, false)->GenerateMesh(true);
        }
    }
}

void ChunkHandler::AddBlock(int chunkX, int chunkY, int chunkZ, int blockX, int blockY, int blockZ, unsigned short newBlockID) {
    std::lock_guard<std::mutex> lock(ChunkMutex);
    Chunk* chunk = GetChunkUnlocked(chunkX, chunkY, chunkZ, false);
    Block& block = chunk->GetBlock(blockX, blockY, blockZ);
    if (block.IsAir() ^ (newBlockID == 0)) {
        int currentBlocks = chunk->GetTotalBlocks();
        if (newBlockID == 0) {
            chunk->SetTotalBlocks(currentBlocks - 1);
        }
        else {
            chunk->SetTotalBlocks(currentBlocks + 1);
        }
    }
    if (block.blockID == newBlockID) {
        CRITICAL("Just replaced a block at chunk position {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "} and block position {" + std::to_string(blockX) + ", " + std::to_string(blockY) + ", " + std::to_string(blockZ) + "} with a new block that has an identical numerical ID {" + std::to_string(newBlockID) + "}. This should currently be impossible, please report a bug if you encounter this, thanks");
        psnip_trap();
    }
    block.SetBlockID(newBlockID);
}

void ChunkHandler::RemoveBlock(int chunkX, int chunkY, int chunkZ, int blockX, int blockY, int blockZ) {
    AddBlock(chunkX, chunkY, chunkZ, blockX, blockY, blockZ, 0);
}

Chunk* ChunkHandler::GetChunkUnlocked(int chunkX, int chunkY, int chunkZ, bool addIfNotFound) {
    OVERRIDE_LOG_NAME("ChunkHandler");
    auto chunk = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
    if (chunk != chunks.end()) {
        return chunk->second.get();
    }
    else {
        //DEBUG("Chunk not found at {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}", Globals::GetInstance().debugMode);
        if (addIfNotFound) {
            Chunk* chunk = AddChunkUnlocked(chunkX, chunkY, chunkZ);
            return chunk;
        } else {
            return defaultChunk.get();
        }
    }
}

Chunk* ChunkHandler::AddChunkUnlocked(int chunkX, int chunkY, int chunkZ) {
    auto chunk = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
    if (chunk == chunks.end()) {
        chunks.emplace(std::make_tuple(chunkX, chunkY, chunkZ), std::make_unique<Chunk>(chunkX, chunkY, chunkZ, *this));
        chunk = chunks.find(std::make_tuple(chunkX, chunkY, chunkZ));
        chunk->second->SetPosition(chunkX, chunkY, chunkZ);
        chunk->second->AllocateChunk();

        return chunk->second.get();
    } else {
        return nullptr;
    }
}

int ChunkHandler::MakeUniqueIDFromVec3(int x, int y, int z) {
    return MakeUniqueIDFromVec2(MakeUniqueIDFromVec2(x, y), z);
}

int ChunkHandler::MakeUniqueIDFromVec2(int x, int y) {
    int a = 0;
    if (x >= 0) {
        a = x * 2;
    } else {
        a = (x * -2) - 1;
    }
    int b = 0;
    if (y >= 0) {
        b = y * 2;
    } else {
        b = (y * -2) - 1;
    }

    return ((a + b) * (a + b + 1) / 2) + b;
}

void ChunkHandler::Delete() {
    std::lock_guard<std::mutex> lock(ChunkMutex);
    for (auto it = chunks.begin(); it != chunks.end(); ++it) {
        auto& chunk = it->second;
        chunk->Delete();
    }

    chunks.clear();
    world.totalMemoryUsage = 0;
}