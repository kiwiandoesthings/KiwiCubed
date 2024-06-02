#include <World.h>


World::World() : totalChunks(0), totalMemoryUsage(0), chunkDataBufferObject(0), indirectBufferObject(0) {
    for (int chunkX = 0; chunkX < worldSize; ++chunkX) {
        for (int chunkY = 0; chunkY < worldSize; ++chunkY) {
            for (int chunkZ = 0; chunkZ < worldSize; ++chunkZ) {
                chunkHandler.AddChunk(chunkX, chunkY, chunkZ);
            }
        }
    }

    drawCount = worldSize * worldSize * worldSize;

    //GLCall(glGenBuffers(1, &chunkDataBufferObject));
    //GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunkDataBufferObject));
    //GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ChunkData) * totalChunks, chunkData.data(), GL_STATIC_DRAW));

    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO));
    GLCall(glGenBuffers(1, &EBO));
    GLCall(glGenBuffers(1, &indirectBufferObject));

    // Make the buffer big enough that we DON'T NEED TO RESIZE IT (for now)
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 65563 * 20000, nullptr, GL_DYNAMIC_DRAW));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 65563 * 20000, nullptr, GL_DYNAMIC_DRAW));
}

void World::Render(Shader shaderProgram) {
    // The rest of this is a work in progress
    drawCount = sizeof(commands);

    GLCall(glBindVertexArray(VAO));
    GLCall(glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferObject));
    GLCall(glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, (GLsizei)commands.size(), 0));


    // Setup the chunk data buffer
    //GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunkDataBufferObject));
}

void World::GenerateWorld() {
    std::cout << "[World Creation / Info] Started generating world" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();

    //float totalChunkMemoryUsage = 0;

    for (int chunkX = 0; chunkX < worldSize; ++chunkX) {
        for (int chunkY = 0; chunkY < worldSize; ++chunkY) {
            for (int chunkZ = 0; chunkZ < worldSize; ++chunkZ) {
                Chunk* chunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ);
                GenerateChunk(chunkX, chunkY, chunkZ, *chunk);
                //totalChunkMemoryUsage += chunk->GetMemoryUsage();
            }
        }
    }

    GLCall(glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferObject));
    GLCall(glBufferData(GL_DRAW_INDIRECT_BUFFER, commands.size() * sizeof(DrawElementsIndirectCommand), commands.data(), GL_STATIC_DRAW));

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0f;

    float chunkGenerationSpeed = static_cast<float>(duration / (worldSize * worldSize * worldSize));

    std::cout << "[World Creation / Info] Finished generating world" << std::endl;
    std::cout << "[World Creation / Info] World creation with chunk count of " << worldSize * worldSize * worldSize << " took " << duration << " ms" << " or roughly " << chunkGenerationSpeed << " ms per chunk (slightly innacurate as empty chunks are skipped). With around " << (int)1000 / chunkGenerationSpeed << " chunks generated, meshed and added per second" << std::endl;
}

void World::GenerateChunk(int chunkX, int chunkY, int chunkZ, Chunk chunk) {
    // Basic procedures for preparing a chunk SetPosition should be done before ANYTHING ELSE, or functions relying on the chunks position will not work properly
    chunk.SetPosition(chunkX, chunkY, chunkZ);
    if (!chunk.isAllocated) {
        chunk.AllocateChunk();
    }
    if (!chunk.isGenerated) {
        chunk.GenerateBlocks();
    }
    if (!chunk.isMeshed) {
        chunk.GenerateMesh(chunkHandler);
    }

    chunkLocations.push_back({chunkX, chunkY, chunkZ});
    
    if (chunk.isEmpty) {
        std::cout << "[World Creation / Info] Chunk (" << chunkX << ", " << chunkY << ", " << chunkZ << ")  is empty, so skipping appending to world mesh" << std::endl;
        return;
    }
    
    // Get the numbers ready
    std::vector<GLfloat> chunkVertices = chunk.GetVertices();
    std::vector<GLuint> chunkIndices = chunk.GetIndices();
    
    int verticesSize = (int)chunkVertices.size() * sizeof(GLfloat);
    int indicesSize = (int)chunkIndices.size() * sizeof(GLuint);
    
    //chunk.SetStartIndex(startIndex = (startIndex == 0) ? startIndex : startIndex + 1);
    //chunk.SetEndIndex(startIndex + verticesSize);
    //chunk.SetChunkIndex(totalChunks);
    
    // We love doing magic here. Here's some magic that makes the glMultiDrawElementsIndirect function work
    
    totalChunks++;
    
    commands.resize(totalChunks);
    
    GLCall(glBindVertexArray(VAO));
    
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0));
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))));
    GLCall(glEnableVertexAttribArray(1));
    
    DrawElementsIndirectCommand drawCommand;
    drawCommand.count = indicesSize / sizeof(GLuint);
    drawCommand.instanceCount = 1;
    drawCommand.firstIndex = latestChunkIndexOffset / sizeof(GLuint);
    drawCommand.baseVertex = latestChunkVertexOffset / sizeof(GLfloat);
    drawCommand.baseInstance = 0;
    commands[totalChunks - 1] = drawCommand;

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, latestChunkVertexOffset, verticesSize, chunkVertices.data()));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, latestChunkIndexOffset, indicesSize, chunkIndices.data()));

    totalMemoryUsage += chunkVertices.size() + chunkIndices.size();
    
    latestChunkVertexOffset += verticesSize;
    latestChunkIndexOffset += indicesSize;
}

Chunk World::GetChunk(int chunkX, int chunkY, int chunkZ) {
    return *chunkHandler.GetChunk(chunkX, chunkY, chunkZ);
}

World::~World() {

}