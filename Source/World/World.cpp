#include "World.h"

#include "ChunkHandler.h"
#include "FastNoise.h"
#include "SingleplayerHandler.h"
#include <chrono>


std::atomic<bool> keepRunning(true);


World::World(unsigned int worldSizeHorizontal, unsigned int worldSizeVertical, SingleplayerHandler* singleplayerHandler) : totalMemoryUsage(0), worldSizeHorizontal(worldSizeHorizontal), worldSizeVertical(worldSizeVertical), chunkHandler(*this), singleplayerHandler(singleplayerHandler) {
    for (unsigned int chunkX = 0; chunkX < worldSizeHorizontal; ++chunkX) {
        for (unsigned int chunkY = 0; chunkY < worldSizeVertical; ++chunkY) {
            for (unsigned int chunkZ = 0; chunkZ < worldSizeHorizontal; ++chunkZ) {
                chunkHandler.AddChunk(chunkX, chunkY, chunkZ);
            }
        }
    }
    
    isWorldAllocated = true;

    // World noise presets? This would be called 'spires' keeping as default for now because I don't want to forget settings
	noise.SetSeed(static_cast<unsigned int>(std::time(nullptr)));
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(5);
    noise.SetFractalLacunarity(2);
    noise.SetFractalGain(0.5f);
    noise.SetFractalWeightedStrength(5.0f);
}

void World::Setup() {
    player.Setup();

    std::ifstream configFile("init_config.json");
    if (!configFile.is_open()) {
        CRITICAL("Could not open the JSON config file, aborting");
		psnip_trap();
    }
    OrderedJSON configJSON;
    configFile >> configJSON;

    player.fov = configJSON["init_settings"]["fov"];

    bool foundPosition = false;
    for (int chunkX = 2; chunkX < worldSizeHorizontal - 2; chunkX++) {
        if (foundPosition) {
             continue;
        }
        for (int chunkZ = 2; chunkZ < worldSizeHorizontal - 2; chunkZ++) {
            if (foundPosition) {
                continue;
            }
            for (int chunkY = worldSizeHorizontal; chunkY >= 0; chunkY--) {
                if (foundPosition) {
                    continue;
                }

                Chunk* chunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ, false);

                if (chunk == nullptr) {
                    continue;
                }
        
                if (!chunk->isGenerated || !chunk->isMeshed || chunk->isEmpty) {
                    continue;
                }
            
                for (unsigned int x = 0; x < chunkSize; ++x) {
                    for (unsigned int z = 0; z < chunkSize; ++z) {
                        int level = chunk->GetHeightmapLevelAt(glm::vec2(x, z));
                        if (level != 0) {
                            player.SetPosition(static_cast<float>((chunk->chunkX * chunkSize) + x), static_cast<float>((chunk->chunkY * chunkSize) + level - player.GetEntityData().physicsBoundingBox.corner1.y) + 1, static_cast<float>((chunk->chunkZ * chunkSize) + z));
                            foundPosition = true;
                        }
                    }
                }
            }
        }
    }

    if (!foundPosition) {
        WARN("Could not find suitable position to spawn player");
    }
}

void World::Render(Shader shaderProgram) {
    if (!isWorldGenerated) {
        return;
    }

    if (!Globals::GetInstance().debugMode) {
        GLCall(glEnable(GL_CULL_FACE));
    }
    shaderProgram.Bind();
    for (auto iterator = chunkHandler.chunks.begin(); iterator != chunkHandler.chunks.end(); ++iterator) {
        auto& chunk = iterator->second;
        if (!chunk->isEmpty) {
            chunk->Render();
        }
    }
    GLCall(glDisable(GL_CULL_FACE));

    assetManager.GetShaderProgram(AssetStringID{"kiwicubed", "entity_shader"})->Bind();
    EntityManager::GetInstance().ForEachEntity([](Entity& entity) {
        entity.Render();
    });
}

void World::GenerateWorld() {
    OVERRIDE_LOG_NAME("World Generation");
    INFO("Generating world");
    
    auto start_time = std::chrono::steady_clock::now();
    for (unsigned int chunkX = 0; chunkX < worldSizeHorizontal; ++chunkX) {
        for (unsigned int chunkY = 0; chunkY < worldSizeVertical; ++chunkY) {
            for (unsigned int chunkZ = 0; chunkZ < worldSizeHorizontal; ++chunkZ) {
                Chunk* chunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ, false);
                GenerateChunk(chunkX, chunkY, chunkZ, chunk, false, chunk);
                totalMemoryUsage += chunk->GetMemoryUsage();
            }
        }
    }

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0f;

    int chunkCount = worldSizeHorizontal * worldSizeVertical * worldSizeHorizontal;
    float chunkGenerationSpeed = static_cast<float>(static_cast<float>(duration) / chunkCount);
    float chunkGenerationSpeedSeconds = 1000.0f / chunkGenerationSpeed;


    INFO("Finished generating world");
    INFO("World creation with chunk count of " + std::to_string(chunkCount) + " took " + std::to_string(duration) + " ms" + " or roughly " + std::to_string(chunkGenerationSpeed) + " ms per chunk (slightly innacurate as empty chunks are skipped)");
    INFO("Around " + std::to_string(chunkGenerationSpeedSeconds) + " chunks generated, meshed and added per second");

    isWorldGenerated = true;

    for (auto it = chunkHandler.chunks.begin(); it != chunkHandler.chunks.end(); ++it) {
        auto& chunk = it->second;
        chunk->SetupRenderComponents();
    }
}

void World::GenerateChunk(int chunkX, int chunkY, int chunkZ, Chunk* chunk, bool updateCallerChunk, Chunk* callerChunk) {
    // Basic procedures for preparing a chunk, SetPosition should be done before ANYTHING ELSE, or functions relying on the chunks position will not work properly
    chunk->SetPosition(chunkX, chunkY, chunkZ);

    if (!chunk->shouldGenerate) {
        return;
    }
    if (!chunk->isAllocated) {
        chunk->AllocateChunk();
    }
    if (!chunk->isGenerated) {
		chunk->GenerateBlocks(*this, chunk, false, false);
	}

    Chunk* positiveXChunk = chunkHandler.GetChunk(chunkX + 1, chunkY, chunkZ, true);     // Positive X
    Chunk* negativeXChunk = chunkHandler.GetChunk(chunkX - 1, chunkY, chunkZ, true);     // Negative X
    Chunk* positiveYChunk = chunkHandler.GetChunk(chunkX, chunkY + 1, chunkZ, true);     // Positive Y
    Chunk* negativeYChunk = chunkHandler.GetChunk(chunkX, chunkY - 1, chunkZ, true);     // Negative Y
    Chunk* positiveZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ + 1, true);     // Positive Z
    Chunk* negativeZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ - 1, true);     // Negative Z

    if (positiveXChunk->isGenerated && negativeXChunk->isGenerated && positiveYChunk->isGenerated && negativeYChunk->isGenerated && positiveZChunk->isGenerated && negativeZChunk->isGenerated && !chunk->isMeshed) {
        chunk->GenerateMesh(false);
    }
    else if (!updateCallerChunk) {
        if (!positiveXChunk->isGenerated) {
            GenerateChunk(chunkX + 1, chunkY, chunkZ, positiveXChunk, true, chunk);
        }
    
        if (!negativeXChunk->isGenerated) {
            GenerateChunk(chunkX - 1, chunkY, chunkZ, negativeXChunk, true, chunk);
        }
    
        if (!positiveYChunk->isGenerated) {
            GenerateChunk(chunkX, chunkY + 1, chunkZ, positiveYChunk, true, chunk);
        }
    
        if (!negativeYChunk->isGenerated) {
            GenerateChunk(chunkX, chunkY - 1, chunkZ, negativeYChunk, true, chunk);
        }
    
        if (!positiveZChunk->isGenerated) {
            GenerateChunk(chunkX, chunkY, chunkZ + 1, positiveZChunk, true, chunk);
        }
    
        if (!negativeZChunk->isGenerated) {
            GenerateChunk(chunkX, chunkY, chunkZ - 1, negativeZChunk, true, chunk);
        }
    }

    if (updateCallerChunk) {
        GenerateChunk(callerChunk->chunkX, callerChunk->chunkY, callerChunk->chunkZ, callerChunk, false, chunk);
    }
}

void World::RecalculateChunksToLoad(EventData& eventData, unsigned short horizontalRadius, unsigned short verticalRadius) {
    if (horizontalRadius == 0) {
        horizontalRadius = playerChunkGenerationRadiusHorizontal;
    }
    if (verticalRadius == 0) {
        verticalRadius = playerChunkGenerationRadiusVertical;
    }
	//WorldPlayerMove moveEvent = *static_cast<WorldPlayerMove*>(eventData.data);
	//glm::vec3 playerChunkPosition = glm::vec3(static_cast<int>(moveEvent.newPlayerX) % 32, static_cast<int>(moveEvent.newPlayerY) % 32, static_cast<int>(moveEvent.newPlayerZ) % 32);
	glm::ivec3 playerChunkPosition = glm::vec3(0, 0, 0);
    std::lock_guard<std::mutex> lock(ChunkQueueMutex);
    {
        for (int chunkX = playerChunkPosition.x - horizontalRadius; chunkX < playerChunkPosition.x + horizontalRadius; ++chunkX) {
            for (int chunkY = playerChunkPosition.y - verticalRadius; chunkY < playerChunkPosition.y + verticalRadius; ++chunkY) {
                for (int chunkZ = playerChunkPosition.z - horizontalRadius; chunkZ < playerChunkPosition.z + horizontalRadius; ++chunkZ) {
                    std::tuple<int, int, int> chunkPosition = {chunkX, chunkY, chunkZ};
                    Chunk* chunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ, false);
                    if (chunkHandler.GetChunkExists(chunkX, chunkY, chunkZ) && chunk->GetMeshable(chunkHandler) && chunk->generationStatus != 3) {
                        if (chunkMeshingSet.find(chunkPosition) != chunkMeshingSet.end()) {
                            continue;
                        }
                        chunkMeshingSet.insert(chunkPosition);
                        chunkMeshingQueue.push_back(glm::ivec3(chunkX, chunkY, chunkZ));
                    } else {
                        if (chunkGenerationSet.find(chunkPosition) != chunkGenerationSet.end()) {
                            continue;
                        }
                        chunkGenerationSet.insert(chunkPosition);
                        chunkGenerationQueue.push_back(glm::ivec3(chunkX, chunkY, chunkZ));
                    }
                }
            }
        }
        
        // this is bad code probably. maybe remove unload-needing chunks from gen/mesh queues in below loop instead of separate loops
        glm::ivec3 playerPosition = player.GetEntityData().globalChunkPosition;
        for (auto iterator = chunkHandler.chunks.begin(); iterator != chunkHandler.chunks.end(); iterator++) {
            auto& chunk = iterator->second;
            int distanceX = playerPosition.x - chunk->chunkX - 1;
            int distanceY = playerPosition.y - chunk->chunkY - 1;
            int distanceZ = playerPosition.z - chunk->chunkZ - 1;

            if (abs(distanceX) > playerChunkGenerationRadiusHorizontal + 2 || abs(distanceY) > playerChunkGenerationRadiusVertical + 2 || abs(distanceZ) > playerChunkGenerationRadiusHorizontal + 2) {            
                chunkUnloadingQueue.push(glm::ivec3(chunk->chunkX, chunk->chunkY, chunk->chunkZ));
            }
        }
          
        for (int iterator = 0; iterator < chunkGenerationQueue.size();) {
            glm::ivec3 chunkPosition = chunkGenerationQueue[iterator];
            int distanceX = playerPosition.x - chunkPosition.x;
            int distanceY = playerPosition.y - chunkPosition.y;
            int distanceZ = playerPosition.z - chunkPosition.z;
          
            if (abs(distanceX) > playerChunkGenerationRadiusHorizontal + 2 || abs(distanceY) > playerChunkGenerationRadiusVertical + 2 || abs(distanceZ) > playerChunkGenerationRadiusHorizontal + 2) {
                chunkGenerationQueue.erase(chunkGenerationQueue.begin() + iterator);
                chunkGenerationSet.erase(std::tuple<int, int, int>(chunkPosition.x, chunkPosition.y, chunkPosition.z));
            } else {
                iterator++;
            }
        }   
          
        for (int iterator = 0; iterator < chunkMeshingQueue.size(); iterator++) {
            glm::ivec3 chunkPosition = chunkMeshingQueue[iterator];
            int distanceX = playerPosition.x - chunkPosition.x;
            int distanceY = playerPosition.y - chunkPosition.y;
            int distanceZ = playerPosition.z - chunkPosition.z;
          
            if (abs(distanceX) > playerChunkGenerationRadiusHorizontal + 2 || abs(distanceY) > playerChunkGenerationRadiusVertical + 2 || abs(distanceZ) > playerChunkGenerationRadiusHorizontal + 2) {
                chunkMeshingQueue.erase(chunkMeshingQueue.begin() + iterator);
                chunkMeshingSet.erase(std::tuple<int, int, int>(chunkPosition.x, chunkPosition.y, chunkPosition.z));
            } else {
                iterator++;
            }
        } 
    }
}

void World::QueueMesh(glm::ivec3 chunkPosition, bool remesh) {
    chunkGenerationThreads.QueueTask([&]{
        GetChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z)->GenerateMesh(true);
    });
}

void World::QueueTickTask(std::function<void()> task) {
    tickTaskQueue.push(task);
}

void World::UpdateFrameTime(std::chrono::steady_clock::time_point newFrameTime) {
    frameTimeSinceTick = std::chrono::duration<float>(newFrameTime - lastTickTime).count();
    partialTicks = frameTimeSinceTick / (tickIntervalMs);
}

unsigned int World::GetTotalTicks() {
    return totalTicks;
}

float World::GetPartialTicks() {
    return partialTicks;
}

void World::SpawnItemFromBlock(glm::ivec3 chunkPosition, glm::ivec3 blockPosition, BlockType* blockType) {
    //entities.push_back(std::make_unique<Entity>(blockPosition.x + (chunkPosition.x * chunkSize) + 0.5, blockPosition.y + (chunkPosition.y * chunkSize) + 0.15, blockPosition.z + (chunkPosition.z * chunkSize) + 0.5, this));
    //Entity* entity = entities[entities.size() - 1].get();
    //entity->SetupRenderComponents(AssetStringID{"kiwicubed", "model/dropped_item"}, AssetStringID{"kiwicubed", "terrain_atlas"}, blockType->metaTextures[0].stringID);
    
}

void World::Update() {
    OVERRIDE_LOG_NAME("World Updating");

	WorldTickEvent tickEvent = WorldTickEvent{totalTicks};
	EventData eventData = EventData{&tickEvent, sizeof(tickEvent)};
	EventManager::GetInstance().TriggerEvent(EVENT_WORLD_TICK, eventData);

    std::vector<glm::ivec3> chunkGenerationQueueCopy;
    std::vector<glm::ivec3> chunkMeshingQueueCopy;
    std::queue<glm::ivec3> chunkUnloadingQueueCopy;
    std::queue<std::function<void()>> tickTaskQueueCopy;

    {
        std::lock_guard<std::mutex> lock(ChunkQueueMutex);
        std::swap(tickTaskQueueCopy, tickTaskQueue);
    }

    while (tickTaskQueueCopy.size() > 0) {
        tickTaskQueueCopy.front()();
        tickTaskQueueCopy.pop();
    }
    
    {
        std::lock_guard<std::mutex> lock(ChunkQueueMutex);
        std::swap(chunkGenerationQueueCopy, chunkGenerationQueue);
        std::swap(chunkMeshingQueueCopy, chunkMeshingQueue);
        std::swap(chunkUnloadingQueueCopy, chunkUnloadingQueue);
    }

    while (chunkGenerationQueueCopy.size() > 0) {
        glm::ivec3 chunkPosition = chunkGenerationQueueCopy.front();
        if (!chunkHandler.GetChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z, false)->isAllocated) {
            chunkHandler.AddChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z);
        }
        chunkHandler.GenerateChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z, nullptr, false, false);
        chunkGenerationQueueCopy.erase(chunkGenerationQueueCopy.begin());
    }

    while (chunkMeshingQueueCopy.size() > 0) {
        glm::ivec3 chunkPosition = chunkMeshingQueueCopy.front();
        chunkHandler.MeshChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z);
        chunkMeshingQueueCopy.erase(chunkMeshingQueueCopy.begin());
    }

    while (chunkUnloadingQueueCopy.size() > 0) {
        glm::ivec3 chunkPosition = chunkUnloadingQueueCopy.front();
        Chunk* chunk = chunkHandler.GetChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z, false);
        chunkUnloadingQueueCopy.pop();
        if (!chunk->IsReal()) {
            WARN("Trying to unload already unloaded chunk at position {" + std::to_string(chunkPosition.x) + ", " + std::to_string(chunkPosition.y) + ", " + std::to_string(chunkPosition.z) + "}, aborting");
            return;
        }
        auto iterator = chunkHandler.chunks.find(std::tuple(chunkPosition.x, chunkPosition.y, chunkPosition.z));
        if (iterator != chunkHandler.chunks.end()) {
            auto& chunk = iterator->second;
            chunk->shouldDelete = true;
        }
        auto generationIterator = chunkGenerationSet.find(std::tuple(chunkPosition.x, chunkPosition.y, chunkPosition.z));
        if (generationIterator != chunkGenerationSet.end()) {
            chunkGenerationSet.erase(generationIterator);
        }
        auto meshingIterator = chunkMeshingSet.find(std::tuple(chunkPosition.x, chunkPosition.y, chunkPosition.z));
        if (meshingIterator != chunkMeshingSet.end()) {
            chunkMeshingSet.erase(meshingIterator);
        }
    }

    {
        std::lock_guard<std::mutex> lock(ChunkQueueMutex);
        std::swap(chunkGenerationQueueCopy, chunkGenerationQueue);
        std::swap(chunkMeshingQueueCopy, chunkMeshingQueue);
        std::swap(chunkUnloadingQueueCopy, chunkUnloadingQueue);
        std::swap(tickTaskQueueCopy, tickTaskQueue);
    }
}

// Pass 0 for world ImGui, 1 for chunk ImGui...
void World::DisplayImGui(unsigned int option) {
    if (option == 0) {
        EntityData playerData = player.GetEntityData();

		ImGui::Text("Player name: %s", player.GetEntityData().name.c_str());
        ImGui::Text("Player AUID: %d", player.GetProtectedEntityData().AUID);
        ImGui::Text("Player gamemode: %s", player.GetGameModeString().c_str());
		ImGui::Text("Player health: %d", static_cast<int>(player.GetEntityStats().health));
		ImGui::Text("Player position: %.2f, %.2f, %.2f",
			playerData.position.x,
			playerData.position.y,
			playerData.position.z);
		ImGui::Text("Player orientation: %.2f, %.2f, %.2f", 
			playerData.orientation.x,
			playerData.orientation.y, 
			playerData.orientation.z);
		ImGui::Text("Player velocity: %.2f, %.2f, %.2f",
			player.GetEntityData().velocity.x,
			player.GetEntityData().velocity.y,
			player.GetEntityData().velocity.z);
        ImGui::Text("Player grounded and jumping state: %d, %d", player.GetEntityData().isGrounded, player.GetEntityData().isJumping);
		ImGui::Text("Global chunk position: %d, %d, %d", 
			static_cast<int>(player.GetEntityData().globalChunkPosition.x), 
			static_cast<int>(player.GetEntityData().globalChunkPosition.y),
			static_cast<int>(player.GetEntityData().globalChunkPosition.z));
		ImGui::Text("Local chunk position: %d, %d, %d", 
			static_cast<int>(player.GetEntityData().localChunkPosition.x), 
			static_cast<int>(player.GetEntityData().localChunkPosition.y),
			static_cast<int>(player.GetEntityData().localChunkPosition.z));
		ImGui::Text("Current chunk generation status and blocks %d, %d",
			GetChunk(
				player.GetEntityData().globalChunkPosition.x,
				player.GetEntityData().globalChunkPosition.y,
				player.GetEntityData().globalChunkPosition.z)->generationStatus,
			GetChunk(
				player.GetEntityData().globalChunkPosition.x,
				player.GetEntityData().globalChunkPosition.y,
				player.GetEntityData().globalChunkPosition.z)->GetTotalBlocks());
    }

    if (option == 1) {
        ImGui::Text("TPS: %.2f", ticksPerSecond);
        ImGui::Text("Until next tick: %.f", tickIntervalMs - tickAccumulator);
        std::queue<std::function<void()>> temporaryQueue = tickTaskQueue;
        int tasks = 0;
        while (!temporaryQueue.empty()) {
            tasks++;
            temporaryQueue.pop();
        }
        ImGui::Text("%d tasks currently queued for tick thread", tasks);
        ImGui::Text("Total chunks: %d", Chunk::totalChunks);
        ImGui::Text("Rough memory usage: %.2f MB", totalMemoryUsage / (1024.0f * 1024.0f));
        if (ImGui::CollapsingHeader("Chunk Generation Queue")) {
            std::vector<glm::ivec3> temporaryQueue = chunkGenerationQueue;
            while (!temporaryQueue.empty()) {
                const glm::ivec3& pos = temporaryQueue.front();
                ImGui::Text("{%d, %d, %d}", pos.x, pos.y, pos.z);
                temporaryQueue.erase(temporaryQueue.begin());
            }
        }
        if (ImGui::CollapsingHeader("Chunk Meshing Queue")) {
            std::vector<glm::ivec3> temporaryQueue = chunkMeshingQueue;
            while (!temporaryQueue.empty()) {
                const glm::ivec3& pos = temporaryQueue.front();
                ImGui::Text("{%d, %d, %d}", pos.x, pos.y, pos.z);
                temporaryQueue.erase(temporaryQueue.begin());
            }
        }
        if (ImGui::CollapsingHeader("Chunk Unloading Queue")) {
            std::queue<glm::ivec3> temporaryQueue = chunkUnloadingQueue;
            while (!temporaryQueue.empty()) {
                const glm::ivec3& pos = temporaryQueue.front();
                ImGui::Text("{%d, %d, %d}", pos.x, pos.y, pos.z);
                temporaryQueue.pop();
            }
        }
    }
    else if (option == 2) {
        for (auto iterator = chunkHandler.chunks.begin(); iterator != chunkHandler.chunks.end(); ++iterator) {
            auto& chunk = iterator->second;
            chunk->DisplayImGui();
        }
    }
}

ChunkHandler& World::GetChunkHandler() {
    return chunkHandler;
}

Chunk* World::GetChunk(int chunkX, int chunkY, int chunkZ) {
    return chunkHandler.GetChunk(chunkX, chunkY, chunkZ, false);
}

Entity* World::GetEntity(std::string uuid) {
    // Later
    return nullptr;
}

Player& World::GetPlayer() {
    return player;
}

std::vector<float>& World::GetChunkDebugVisualizationVertices() {
    chunkDebugVisualizationVertices.clear();
    std::lock_guard<std::mutex> lock(chunkHandler.ChunkMutex);
    for (auto iterator = chunkHandler.chunks.begin(); iterator != chunkHandler.chunks.end(); ++iterator) {
        auto& chunk = iterator->second;
        chunkDebugVisualizationVertices.insert(chunkDebugVisualizationVertices.end(), chunk->GetDebugVisualizationVertices().begin(), chunk->GetDebugVisualizationVertices().end());
    }
    return chunkDebugVisualizationVertices;
}

std::vector<GLuint>& World::GetChunkDebugVisualizationIndices() {
    chunkDebugVisualizationIndices.clear();
    std::lock_guard<std::mutex> lock(chunkHandler.ChunkMutex);
    for (auto iterator = chunkHandler.chunks.begin(); iterator != chunkHandler.chunks.end(); ++iterator) {
        auto& chunk = iterator->second;
        chunkDebugVisualizationIndices.insert(chunkDebugVisualizationIndices.end(), chunk->GetDebugVisualizationIndices().begin(), chunk->GetDebugVisualizationIndices().end());
    }
    return chunkDebugVisualizationIndices;
}

std::vector<glm::vec4>& World::GetChunkOrigins() {
    chunkOrigins.clear();
    std::lock_guard<std::mutex> lock(chunkHandler.ChunkMutex);
    for (auto iterator = chunkHandler.chunks.begin(); iterator != chunkHandler.chunks.end(); ++iterator) {
        auto& chunk = iterator->second;
        chunkOrigins.emplace_back(glm::vec4(chunk->chunkX * chunkSize + static_cast<int>(chunkSize / 2), chunk->chunkY * chunkSize + static_cast<int>(chunkSize / 2), chunk->chunkZ * chunkSize + static_cast<int>(chunkSize / 2), chunk->generationStatus));
    }
    return chunkOrigins;
}

FastNoiseLite& World::GetNoise() {
    return noise;
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
        INFO("Tick thread stopped");
    } else {
        WARN("Tick thread was not joinable (could not be stopped)");
    }
    return true;
}

void World::Tick() {
    std::lock_guard<std::mutex> lock(tickThreadMutex);

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - lastTickTime).count();
    if (totalTicks == 1) {
        totalMemoryUsage = 0;
        for (auto iterator = chunkHandler.chunks.begin(); iterator != chunkHandler.chunks.end(); ++iterator) {
            auto& chunk = iterator->second;
            totalMemoryUsage += chunk->GetMemoryUsage();
        }
    }

    Update();

    if (duration >= 1000.0f) {
        ticksPerSecond = static_cast<float>(totalTicks) / (duration / 1000.0f);
        totalTicks = 0;
        lastTickTime = end_time;
    }

    ++totalTicks;
}

void World::RunTickThread() {
    auto nextTickTime = std::chrono::steady_clock::now();

    while (shouldTick) {
        auto currentTime = std::chrono::steady_clock::now();
        auto frameTime = std::chrono::duration<double, std::milli>(currentTime - nextTickTime).count();
        nextTickTime = currentTime;

        tickAccumulator += frameTime;

        while (tickAccumulator >= tickIntervalMs) {
            Tick();
            tickAccumulator -= tickIntervalMs;
        }
    }
}

void World::Delete() {
    StopTickThread();
    chunkHandler.Delete();
    chunkGenerationThreads.Delete();
    player.Delete();
}