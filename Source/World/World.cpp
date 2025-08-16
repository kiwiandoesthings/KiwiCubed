#include "World.h"

#include "SingleplayerHandler.h"


std::atomic<bool> keepRunning(true);


World::World(unsigned int worldSize, SingleplayerHandler* singleplayerHandler) : totalChunks(0), totalMemoryUsage(0), singleplayerHandler(singleplayerHandler), worldSize(worldSize), chunkHandler(*this) {
    player.SetPosition(static_cast<int>(worldSize * chunkSize / 2), static_cast<int>(worldSize * chunkSize / 2), static_cast<int>(worldSize * chunkSize / 2));
    for (unsigned int chunkX = 0; chunkX < worldSize; ++chunkX) {
        for (unsigned int chunkY = 0; chunkY < worldSize; ++chunkY) {
            for (unsigned int chunkZ = 0; chunkZ < worldSize; ++chunkZ) {
                chunkHandler.AddChunk(chunkX, chunkY, chunkZ);
            }
        }
    }
    
    isWorldAllocated = true;
}

void World::Setup(Window& window) {
    player.Setup(window);
}

void World::Render(Shader shaderProgram) {
    if (!isWorldGenerated) {
        return;
    }

    shaderProgram.Bind();
    for (auto iterator = chunkHandler.chunks.begin(); iterator != chunkHandler.chunks.end(); ++iterator) {
        auto& chunk = iterator->second;
        if (!chunk.isEmpty) {
            chunk.Render();
        }
    }
}

void World::GenerateWorld() {
    OVERRIDE_LOG_NAME("World Generation");
    INFO("Generating world");
    
    auto start_time = std::chrono::high_resolution_clock::now();
    for (unsigned int chunkX = 0; chunkX < worldSize; ++chunkX) {
        for (unsigned int chunkY = 0; chunkY < worldSize; ++chunkY) {
            for (unsigned int chunkZ = 0; chunkZ < worldSize; ++chunkZ) {
                Chunk& chunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ, false);
                GenerateChunk(chunkX, chunkY, chunkZ, chunk, false, chunk);
                totalMemoryUsage += chunk.GetMemoryUsage();
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0f;

    int chunkCount = worldSize * worldSize * worldSize;
    float chunkGenerationSpeed = static_cast<float>(static_cast<float>(duration) / chunkCount);
    float chunkGenerationSpeedSeconds = 1000.0f / chunkGenerationSpeed;


    INFO("Finished generating world");
    INFO("World creation with chunk count of " + std::to_string(chunkCount) + " took " + std::to_string(duration) + " ms" + " or roughly " + std::to_string(chunkGenerationSpeed) + " ms per chunk (slightly innacurate as empty chunks are skipped)");
    INFO("Around " + std::to_string(chunkGenerationSpeedSeconds) + " chunks generated, meshed and added per second");

    isWorldGenerated = true;

    for (auto it = chunkHandler.chunks.begin(); it != chunkHandler.chunks.end(); ++it) {
        auto& chunk = it->second;
        chunk.SetupRenderComponents();
    }

    singleplayerHandler->isLoadedIntoSingleplayerWorld = true;
}

void World::GenerateChunk(int chunkX, int chunkY, int chunkZ, Chunk& chunk, bool updateCallerChunk, Chunk& callerChunk) {
    // Basic procedures for preparing a chunk, SetPosition should be done before ANYTHING ELSE, or functions relying on the chunks position will not work properly
    chunk.SetPosition(chunkX, chunkY, chunkZ);

    if (!chunk.shouldGenerate) {
        return;
    }
    if (!chunk.isAllocated) {
        chunk.AllocateChunk();
    }
    if (!chunk.isGenerated) {
		chunk.GenerateBlocks(*this, chunk, false, false);
	}

    totalChunks++;

    Chunk& positiveXChunk = chunkHandler.GetChunk(chunkX + 1, chunkY, chunkZ, true);     // Positive X
    Chunk& negativeXChunk = chunkHandler.GetChunk(chunkX - 1, chunkY, chunkZ, true);     // Negative X
    Chunk& positiveYChunk = chunkHandler.GetChunk(chunkX, chunkY + 1, chunkZ, true);     // Positive Y
    Chunk& negativeYChunk = chunkHandler.GetChunk(chunkX, chunkY - 1, chunkZ, true);     // Negative Y
    Chunk& positiveZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ + 1, true);     // Positive Z
    Chunk& negativeZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ - 1, true);     // Negative Z

    if (positiveXChunk.isGenerated && negativeXChunk.isGenerated && positiveYChunk.isGenerated && negativeYChunk.isGenerated && positiveZChunk.isGenerated && negativeZChunk.isGenerated && !chunk.isMeshed) {
        chunk.GenerateMesh(chunkHandler, false);
    }
    else if (!updateCallerChunk) {
        if (!positiveXChunk.isGenerated) {
            GenerateChunk(chunkX + 1, chunkY, chunkZ, positiveXChunk, true, chunk);
        }
    
        if (!negativeXChunk.isGenerated) {
            GenerateChunk(chunkX - 1, chunkY, chunkZ, negativeXChunk, true, chunk);
        }
    
        if (!positiveYChunk.isGenerated) {
            GenerateChunk(chunkX, chunkY + 1, chunkZ, positiveYChunk, true, chunk);
        }
    
        if (!negativeYChunk.isGenerated) {
            GenerateChunk(chunkX, chunkY - 1, chunkZ, negativeYChunk, true, chunk);
        }
    
        if (!positiveZChunk.isGenerated) {
            GenerateChunk(chunkX, chunkY, chunkZ + 1, positiveZChunk, true, chunk);
        }
    
        if (!negativeZChunk.isGenerated) {
            GenerateChunk(chunkX, chunkY, chunkZ - 1, negativeZChunk, true, chunk);
        }
    }

    if (updateCallerChunk) {
        GenerateChunk(callerChunk.chunkX, callerChunk.chunkY, callerChunk.chunkZ, callerChunk, false, chunk);
    }
}

void World::GenerateChunksAroundPosition(Event& event, unsigned short horizontalRadius, unsigned short verticalRadius) {
    if (horizontalRadius == 0) {
        horizontalRadius = playerChunkGenerationRadius;
    }
    if (verticalRadius == 0) {
        verticalRadius = playerChunkGenerationRadius;
    }
    auto* playerChunkPosition = event.GetData<glm::ivec3>("globalChunkPosition");
    if (playerChunkPosition == nullptr) {
        return;
    }

    for (int chunkX = playerChunkPosition->x - horizontalRadius; chunkX < playerChunkPosition->x + horizontalRadius; ++chunkX) {
        for (int chunkY = playerChunkPosition->y - verticalRadius; chunkY < playerChunkPosition->y + verticalRadius; ++chunkY) {
            for (int chunkZ = playerChunkPosition->z - horizontalRadius; chunkZ < playerChunkPosition->z + horizontalRadius; ++chunkZ) {
                std::tuple<int, int, int> chunkPosition = {chunkX, chunkY, chunkZ};
                Chunk& chunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ, false);
                if (chunkHandler.GetChunkExists(chunkX, chunkY, chunkZ) && chunk.GetMeshable(chunkHandler) && chunk.generationStatus != 3) {
                    if (chunkMeshingSet.find(chunkPosition) == chunkMeshingSet.end()) {
                        chunkMeshingSet.insert(chunkPosition);
                        chunkMeshingQueue.push(glm::ivec3(chunkX, chunkY, chunkZ));
                        meshingQueuedChunks++;
                    }
                } else {
                    if (chunkGenerationSet.find(chunkPosition) == chunkGenerationSet.end()) {
                        chunkGenerationSet.insert(chunkPosition);
                        chunkGenerationQueue.push(glm::ivec3(chunkX, chunkY, chunkZ));
                        generationQueuedChunks++;
                    }
                }
            }
        }
    }
}

void World::Update() {
    //player.Update();

    if (generationQueuedChunks > 0) {
        chunkHandler.AddChunk(chunkGenerationQueue.front().x, chunkGenerationQueue.front().y, chunkGenerationQueue.front().z);
        chunkHandler.GenerateChunk(chunkGenerationQueue.front().x, chunkGenerationQueue.front().y, chunkGenerationQueue.front().z, defaultChunk, false, false);
        chunkGenerationQueue.pop();
        generationQueuedChunks--;
    }

    if (meshingQueuedChunks > 0) {
        bool meshed = chunkHandler.MeshChunk(chunkMeshingQueue.front().x, chunkMeshingQueue.front().y, chunkMeshingQueue.front().z);
        if (meshed) {
            chunkMeshingQueue.pop();
            meshingQueuedChunks--;
        }
    }
}

// Pass 0 for world ImGui, 1 for chunk ImGui...
void World::DisplayImGui(unsigned int option) {
    if (option == 0) {
        ImGui::Text("TPS: %d", ticksPerSecond);
        ImGui::Text("Total chunks: %d", totalChunks);
        ImGui::Text("Memory usage: %.2f MB", totalMemoryUsage / (1024.0 * 1024.0));
        if (ImGui::CollapsingHeader("Chunk Generation Queue")) {
            std::queue<glm::ivec3> temporaryQueue = chunkGenerationQueue;
            while (!temporaryQueue.empty()) {
                const glm::ivec3& pos = temporaryQueue.front();
                ImGui::Text("{%d, %d, %d}", pos.x, pos.y, pos.z);
                temporaryQueue.pop();
            }
        }
        if (ImGui::CollapsingHeader("Chunk Meshing Queue")) {
            std::queue<glm::ivec3> temporaryQueue = chunkMeshingQueue;
            while (!temporaryQueue.empty()) {
                const glm::ivec3& pos = temporaryQueue.front();
                ImGui::Text("{%d, %d, %d}", pos.x, pos.y, pos.z);
                temporaryQueue.pop();
            }
        }
    }
    else if (option == 1) {
        for (auto iterator = chunkHandler.chunks.begin(); iterator != chunkHandler.chunks.end(); ++iterator) {
            auto& chunk = iterator->second;
            chunk.DisplayImGui();
        }
    }
}

Chunk World::GetChunk(int chunkX, int chunkY, int chunkZ) {
    return chunkHandler.GetChunk(chunkX, chunkY, chunkZ, false);
}

Entity World::GetEntity(std::string uuid) {
    // Later
    return Entity(0, 0, 0);
}

std::vector<float>& World::GetChunkDebugVisualizationVertices() {
    chunkDebugVisualizationVertices.clear();
    for (auto it = chunkHandler.chunks.begin(); it != chunkHandler.chunks.end(); ++it) {
        auto& chunk = it->second;
        chunkDebugVisualizationVertices.insert(chunkDebugVisualizationVertices.end(), chunk.GetDebugVisualizationVertices().begin(), chunk.GetDebugVisualizationVertices().end());
    }
    return chunkDebugVisualizationVertices;
}

std::vector<GLuint>& World::GetChunkDebugVisualizationIndices() {
    chunkDebugVisualizationIndices.clear();
    for (auto it = chunkHandler.chunks.begin(); it != chunkHandler.chunks.end(); ++it) {
        auto& chunk = it->second;
        chunkDebugVisualizationIndices.insert(chunkDebugVisualizationIndices.end(), chunk.GetDebugVisualizationIndices().begin(), chunk.GetDebugVisualizationIndices().end());
    }
    return chunkDebugVisualizationIndices;
}

std::vector<glm::vec4>& World::GetChunkOrigins() {
    chunkOrigins.clear();
    for (auto it = chunkHandler.chunks.begin(); it != chunkHandler.chunks.end(); ++it) {
        auto& chunk = it->second;
        chunkOrigins.emplace_back(glm::vec4(chunk.chunkX * chunkSize + static_cast<int>(chunkSize / 2), chunk.chunkY * chunkSize + static_cast<int>(chunkSize / 2), chunk.chunkZ * chunkSize + static_cast<int>(chunkSize / 2), chunk.generationStatus));
    }
    return chunkOrigins;
}

bool World::StartTickThread() {
    OVERRIDE_LOG_NAME("World Tick Thread");
    if (shouldTick) {
        WARN("ried to start tick thread while it was running, aborting");
        return false;
    }

    INFO("Starting tick thread");
    shouldTick = true;
    tickThread = std::thread(&World::RunTickThread, this);
    return true;
}

bool World::StopTickThread() {
    OVERRIDE_LOG_NAME("World Tick Thread");
    if (!shouldTick) {
        WARN("Tried to stop tick thread while it was stopped, aborting");
        return false;
    }

    INFO("Stopping tick thread");
    shouldTick = false;
    if (tickThread.joinable()) {
        tickThread.join();
    } else {
        WARN("Tick thread was not joinable (could not be stopped)");
    }
    return true;
}

void World::Tick() {
    std::lock_guard<std::mutex> lock(tickThreadMutex);

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - tpsStartTime).count();
    if (totalTicks == 1) {
        totalMemoryUsage = 0;
        for (auto iterator = chunkHandler.chunks.begin(); iterator != chunkHandler.chunks.end(); ++iterator) {
            auto& chunk = iterator->second;
            totalMemoryUsage += chunk.GetMemoryUsage();
        }
    }

    Update();

    if (duration >= 1000.0) {
        ticksPerSecond = static_cast<unsigned int>(static_cast<float>(totalTicks) / (duration / 1000.0));
        totalTicks = 0;
        tpsStartTime = end_time;
    }

    ++totalTicks;
}

void World::RunTickThread() {
    while (shouldTick) {
        auto nextTickTime = std::chrono::steady_clock::now();
        Tick();
        nextTickTime += std::chrono::milliseconds(tickIntervalMs);
        std::this_thread::sleep_until(nextTickTime);
    }
}

void World::Delete() {
    StopTickThread();
    chunkHandler.Delete();
    chunkGenerationThreads.Delete();
}