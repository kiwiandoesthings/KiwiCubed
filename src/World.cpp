#include "World.h"

#include "SingleplayerHandler.h"


std::atomic<bool> keepRunning(true);


World::World(unsigned int worldSize, SingleplayerHandler& singleplayerHandler) : totalChunks(0), totalMemoryUsage(0), chunkHandler(*this), worldSize(worldSize), singleplayerHandler(singleplayerHandler) {
    for (unsigned int chunkX = 0; chunkX < worldSize; ++chunkX) {
        for (unsigned int chunkY = 0; chunkY < worldSize; ++chunkY) {
            for (unsigned int chunkZ = 0; chunkZ < worldSize; ++chunkZ) {
                totalChunks++;
                chunkHandler.AddChunk(chunkX, chunkY, chunkZ);
            }
        }
    }
    
    isWorldAllocated = true;
}

void World::Setup(Window& window) {
    player.Setup(window);
}

// Currently just calls SetupRenderComponents on all the chunks
void World::SetupRenderComponents() {
    for (auto it = chunkHandler.chunks.begin(); it != chunkHandler.chunks.end(); ++it) {
        const auto& key = it->first;
        auto& chunk = it->second;
        chunk.SetupRenderComponents();
    }

    std::cout << "[World Creation / Info] Finished setting up chunk render components" << std::endl;

    singleplayerHandler.isLoadedIntoSingleplayerWorld = true;
}

void World::Render(Shader shaderProgram) {
    if (!isWorldGenerated) {
        return;
    }

    shaderProgram.Bind();
    for (auto it = chunkHandler.chunks.begin(); it != chunkHandler.chunks.end(); ++it) {
        const auto& key = it->first;
        auto& chunk = it->second;
        if (!chunk.isEmpty) {
            chunk.Render();
        }
    }
}

void World::GenerateWorld() {
    std::cout << "[World Creation / Info] Started generating world" << std::endl;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    for (unsigned int chunkX = 0; chunkX < worldSize; ++chunkX) {
        for (unsigned int chunkY = 0; chunkY < worldSize; ++chunkY) {
            for (unsigned int chunkZ = 0; chunkZ < worldSize; ++chunkZ) {
                Chunk chunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ);
                GenerateChunk(chunkX, chunkY, chunkZ, chunk, false, chunk);
                totalMemoryUsage += chunk.GetMemoryUsage();
            }
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0f;
    
    float chunkGenerationSpeed = static_cast<float>(duration / (worldSize * worldSize * worldSize));
    
    std::cout << std::endl << "[World Creation / Info] Finished generating world" << std::endl;
    std::cout << "[World Creation / Info] World creation with chunk count of " << worldSize * worldSize * worldSize << " took " << duration << " ms" << " or roughly " << chunkGenerationSpeed << " ms per chunk (slightly innacurate as empty chunks are skipped). With around " << static_cast<int>(1000) / chunkGenerationSpeed << " chunks generated, meshed and added per second" << std::endl;
    
    isWorldGenerated = true;
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
        //std::cout << "[Debug] GENERATING {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
		chunk.GenerateBlocks(*this, chunk, false, false);
	}

    totalChunks++;

    Chunk& positiveXChunk = chunkHandler.GetChunk(chunkX + 1, chunkY, chunkZ);     // Positive X
    Chunk& negativeXChunk = chunkHandler.GetChunk(chunkX - 1, chunkY, chunkZ);     // Negative X
    Chunk& positiveYChunk = chunkHandler.GetChunk(chunkX, chunkY + 1, chunkZ);     // Positive Y
    Chunk& negativeYChunk = chunkHandler.GetChunk(chunkX, chunkY - 1, chunkZ);     // Negative Y
    Chunk& positiveZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ + 1);     // Positive Z
    Chunk& negativeZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ - 1);     // Negative Z

    if (positiveXChunk.isGenerated && negativeXChunk.isGenerated && positiveYChunk.isGenerated && negativeYChunk.isGenerated && positiveZChunk.isGenerated && negativeZChunk.isGenerated && !chunk.isMeshed) {
        chunk.GenerateMesh(chunkHandler, false);
    }
    else if (!updateCallerChunk) {
        if (!positiveXChunk.isGenerated) {
            //std::cout << "[Debug] ==1== Going for {" << chunkX + 1 << ", " << chunkY << ", " << chunkZ << "} From: {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
            GenerateChunk(chunkX + 1, chunkY, chunkZ, positiveXChunk, true, chunk);
        }
    
        if (!negativeXChunk.isGenerated) {
            //::cout << "[Debug] ==2== Going for {" << chunkX - 1 << ", " << chunkY << ", " << chunkZ << "} From: {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
            GenerateChunk(chunkX - 1, chunkY, chunkZ, negativeXChunk, true, chunk);
        }
    
        if (!positiveYChunk.isGenerated) {
            //std::cout << "[Debug] ==3== Going for {" << chunkX << ", " << chunkY + 1 << ", " << chunkZ << "} From: {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
            GenerateChunk(chunkX, chunkY + 1, chunkZ, positiveYChunk, true, chunk);
        }
    
        if (!negativeYChunk.isGenerated) {
            //std::cout << "[Debug] ==4== Going for {" << chunkX << ", " << chunkY - 1 << ", " << chunkZ << "} From: {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
            GenerateChunk(chunkX, chunkY - 1, chunkZ, negativeYChunk, true, chunk);
        }
    
        if (!positiveZChunk.isGenerated) {
            //std::cout << "[Debug] ==5== Going for {" << chunkX << ", " << chunkY << ", " << chunkZ + 1 << "} From: {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
            GenerateChunk(chunkX, chunkY, chunkZ + 1, positiveZChunk, true, chunk);
        }
    
        if (!negativeZChunk.isGenerated) {
            //std::cout << "[Debug] ==6== Going for {" << chunkX << ", " << chunkY << ", " << chunkZ - 1 << "} From: {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
            GenerateChunk(chunkX, chunkY, chunkZ - 1, negativeZChunk, true, chunk);
        }
    }

    if (updateCallerChunk) {
        GenerateChunk(callerChunk.chunkX, callerChunk.chunkY, callerChunk.chunkZ, callerChunk, false, chunk);
    }
}

void World::Update(Window* window) {
    player.Update(window);
}

// Pass 0 for world ImGui, 1 for chunk ImGui...
void World::DisplayImGui(unsigned int option) {
    if (option == 0) {
        ImGui::Text("TPS: %d", ticksPerSecond);
        ImGui::Text("Total chunks: %d", totalChunks);
        ImGui::Text("Memory usage: %.2f MB", totalMemoryUsage / (1024.0 * 1024.0));
    }
    else if (option == 1) {
        for (auto it = chunkHandler.chunks.begin(); it != chunkHandler.chunks.end(); ++it) {
            const auto& key = it->first;
            auto& chunk = it->second;
            chunk.DisplayImGui();
        }
    }
}

Chunk World::GetChunk(int chunkX, int chunkY, int chunkZ) {
    return chunkHandler.GetChunk(chunkX, chunkY, chunkZ);
}

Entity World::GetEntity(std::string uuid) {
    // Later
    return Entity(0, 0, 0);
}

bool World::StartTickThread() {
    if (shouldTick) {
        std::cout << "[World Tick Thread / Info] Tried to start tick thread while it was running, aborting" << std::endl;
        return false;
    }

    std::cout << "[World Tick Thread / Info] Starting tick thread" << std::endl;
    shouldTick = true;
    TickThread = std::thread(&World::RunTickThread, this);
    return true;
}

bool World::StopTickThread() {
    if (!shouldTick) {
        std::cout << "[World Tick Thread / Info] Tried to stop tick thread while it was stopped, aborting" << std::endl;
        return false;
    }

    std::cout << "[World Tick Thread / Info] Stopping tick thread" << std::endl;
    shouldTick = false;
    if (TickThread.joinable()) {
        TickThread.join();
    }
    return true;
}

void World::Tick() {
    std::lock_guard<std::mutex> lock(TickThreadMutex);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - tpsStartTime).count();

    if (duration >= 1000.0) {
        ticksPerSecond = (float)totalTicks / (duration / 1000.0);
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
    chunkHandler.Delete();
    StopTickThread();
}