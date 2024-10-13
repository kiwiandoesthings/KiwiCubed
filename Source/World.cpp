#include "World.h"


World::World() : totalChunks(0), totalMemoryUsage(0) {
    for (unsigned int chunkX = 0; chunkX < worldSize; ++chunkX) {
        for (unsigned int chunkY = 0; chunkY < worldSize; ++chunkY) {
            for (unsigned int chunkZ = 0; chunkZ < worldSize; ++chunkZ) {
                chunkHandler.AddChunk(chunkX, chunkY, chunkZ);
            }
        }
    }
}

void World::Setup(Window& window) {
    player.Setup(window);
}

void World::Render(Shader shaderProgram) {
    for (unsigned int chunkX = 0; chunkX < worldSize; ++chunkX) {
        for (unsigned int chunkY = 0; chunkY < worldSize; ++chunkY) {
            for (unsigned int chunkZ = 0; chunkZ < worldSize; ++chunkZ) {
                Chunk& chunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ);
                shaderProgram.Bind();
                chunk.Render();
            }
        }
    }
}

void World::GenerateWorld() {
    std::cout << "[World Creation / Info] Started generating world" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();

    //float totalChunkMemoryUsage = 0;
    for (unsigned int chunkX = 0; chunkX < worldSize; ++chunkX) {
        for (unsigned int chunkY = 0; chunkY < worldSize; ++chunkY) {
            for (unsigned int chunkZ = 0; chunkZ < worldSize; ++chunkZ) {
                Chunk& chunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ);
                GenerateChunk(chunkX, chunkY, chunkZ, chunk, false, emptyChunk);
                //totalChunkMemoryUsage += chunk->GetMemoryUsage();
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0f;

    float chunkGenerationSpeed = static_cast<float>(duration / (worldSize * worldSize * worldSize));

    std::cout << std::endl << "[World Creation / Info] Finished generating world" << std::endl;
    std::cout << "[World Creation / Info] World creation with chunk count of " << worldSize * worldSize * worldSize << " took " << duration << " ms" << " or roughly " << chunkGenerationSpeed << " ms per chunk (slightly innacurate as empty chunks are skipped). With around " << static_cast<int>(1000) / chunkGenerationSpeed << " chunks generated, meshed and added per second" << std::endl;
}

void World::GenerateChunk(int chunkX, int chunkY, int chunkZ, Chunk& chunk, bool updateCallerChunk, Chunk& callerChunk) {
    //std::cout << "===== {" << chunkX << ", " << chunkY << ", " << chunkZ << "} =====" << std::endl;
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
		chunk.GenerateBlocks(*this, chunk, false);
	}

    Chunk& positiveXChunk = chunkHandler.GetChunk(chunkX + 1, chunkY, chunkZ);     // Positive X
    Chunk& negativeXChunk = chunkHandler.GetChunk(chunkX - 1, chunkY, chunkZ);     // Negative X
    Chunk& positiveYChunk = chunkHandler.GetChunk(chunkX, chunkY + 1, chunkZ);     // Positive Y
    Chunk& negativeYChunk = chunkHandler.GetChunk(chunkX, chunkY - 1, chunkZ);     // Negative Y
    Chunk& positiveZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ + 1);     // Positive Z
    Chunk& negativeZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ - 1);     // Negative Z

    if (static_cast<unsigned int>(chunkX) > 0 && static_cast<unsigned int>(chunkX) < worldSize - 1 && static_cast<unsigned int>(chunkY) > 0 && static_cast<unsigned int>(chunkY) < worldSize - 1 && static_cast<unsigned int>(chunkZ) > 0 && static_cast<unsigned int>(chunkZ) < worldSize - 1) {
        if (positiveXChunk.isGenerated && negativeXChunk.isGenerated && positiveYChunk.isGenerated && negativeYChunk.isGenerated && positiveZChunk.isGenerated && negativeZChunk.isGenerated && !chunk.isMeshed) {
            //std::cout << "[Debug] ==MESHING== {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
            chunk.GenerateMesh(chunkHandler);
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
    }

    if (updateCallerChunk) {
        GenerateChunk(callerChunk.chunkX, callerChunk.chunkY, callerChunk.chunkZ, callerChunk, false, chunk);
    }
}

void World::Update(Window* window) {
    player.Update(window, chunkHandler);
}

void World::DisplayImGui() {
    for (unsigned int chunkX = 0; chunkX < worldSize; ++chunkX) {
        for (unsigned int chunkY = 0; chunkY < worldSize; ++chunkY) {
            for (unsigned int chunkZ = 0; chunkZ < worldSize; ++chunkZ) {
                Chunk& chunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ);
                chunk.DisplayImGui();
            }
        }
    }
}

void World::Delete() {

}