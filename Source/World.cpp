#include <World.h>


World::World() : totalChunks(0), totalMemoryUsage(0), chunkDataBufferObject(0), indirectBufferObject(0) {
    for (int x = 0; x < worldSize; ++x) {
        for (int y = 0; y < worldSize; ++y) {
            for (int z = 0; z < worldSize; ++z) {
                // Insert the postion of the chunk and a new chunk instance to the map
                chunkHandler.chunks.insert(std::make_pair(std::tuple(x, y, z), Chunk(x, y, z)));
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

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 122880 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36864 * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW));
}

void World::Render(Shader shaderProgram) {
    //renderer.DrawElements(VAO, vertexBufferObject, indexBufferObject, vertices, indices);

    // Deprecated rendering right now
    //vertexArrayObject.Bind();
    //vertexBufferObject.Bind();
    //vertexBufferObject.Setup(vertices.size() * sizeof(GLfloat), vertices.data());
    //indexBufferObject.Bxind();
    //indexBufferObject.Setup(indices.size() * sizeof(GLuint), indices.data());
    //vertexArrayObject.LinkAttribute(vertexBufferObject, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    //vertexArrayObject.LinkAttribute(vertexBufferObject, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    //GLCall(glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0));

    //for (int x = 0; x < worldSize; ++x) {
	//	for (int y = 0; y < worldSize; ++y) {
	//		for (int z = 0; z < worldSize; ++z) {
	//			chunkHandler.chunks.find(std::make_tuple(x, y, z))->second.Render(shaderProgram);
	//		}
	//	}
	//}

    // The rest of this is a work in progress

    drawCount = totalChunks;

    // Draw both objects using glMultiDrawElementsIndirect
    GLCall(glBindVertexArray(VAO));
    GLCall(glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferObject));
    GLCall(glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, (GLsizei)commands.size(), 0));
    GLCall(glBindVertexArray(0));

    // Setup the chunk data buffer
    //GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunkDataBufferObject));


}

void World::GenerateWorld() {
    auto start_time = std::chrono::high_resolution_clock::now();

    float totalChunkMemoryUsage = 0;

    for (int chunkX = 0; chunkX < worldSize; ++chunkX) {
        for (int chunkY = 0; chunkY < worldSize; ++chunkY) {
            for (int chunkZ = 0; chunkZ < worldSize; ++chunkZ) {
                Chunk chunk = chunkHandler.chunks.find(std::make_tuple(chunkX, chunkY, chunkZ))->second;
                GenerateChunk(chunkX, chunkY, chunkZ, chunk);
                totalChunkMemoryUsage += chunk.GetMemoryUsage();
            }
        }
    }

    GLCall(glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferObject));
    GLCall(glBufferData(GL_DRAW_INDIRECT_BUFFER, commands.size() * sizeof(DrawElementsIndirectCommand), commands.data(), GL_STATIC_DRAW));

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0f;

    float chunkGenerationSpeed = static_cast<float>(duration / (worldSize * worldSize * worldSize));

    std::cout << "World Creation / Info: Total world preparation with chunk count of " << worldSize * worldSize * worldSize << " took " << duration << " ms" << " or roughly " << chunkGenerationSpeed << " ms per chunk (slightly innacurate as empty chunks are skipped). With around " << (int)1000 / chunkGenerationSpeed << " chunks generated, meshed and added per second" << std::endl;
}

void World::GenerateChunk(int chunkX, int chunkY, int chunkZ, Chunk chunk) {
    if (commands.size() > 0) {
        //return;
    }
    // Basic procedures for preparing a chunk SetPosition should be done before ANYTHING ELSE, or functions relying on the chunks position will not work properly
    chunk.SetPosition(chunkX, chunkY, chunkZ);
    if (!chunk.isAllocated) {
        chunk.AllocateChunk();
    }
    if (!chunk.isMeshed) {
        chunk.GenerateMesh(chunkHandler);
    }

    chunkLocations.push_back({chunkX, chunkY, chunkZ});

    if (chunk.isEmpty) {
        std::cout << "World Creation / Info: Chunk is empty, so skipping appending to world mesh" << std::endl;
        return;
    }

    // Get the numbers ready
    std::vector<GLfloat> chunkVertices = chunk.GetVertices();
    std::vector<GLuint> chunkIndices = chunk.GetIndices();

    int startIndex = (int)vertices.size() / 3;
    int numVertices = (int)chunkVertices.size() / 3;
    int numIndices = (int)chunkIndices.size();

    // Add vertices and indices from the chunk
    vertices.insert(vertices.end(), chunkVertices.begin(), chunkVertices.end());
    indices.insert(indices.end(), chunkIndices.begin(), chunkIndices.end());

    chunk.SetStartIndex(startIndex = (startIndex == 0) ? startIndex : startIndex + 1);
    chunk.SetEndIndex(startIndex + numVertices);
    
    // We love doing magic here. Here's some magic that makes the glMultiDrawElementsIndirect function work
    chunk.SetChunkIndex(totalChunks);
    
    int indicesCount = numIndices / sizeof(GLuint);
    
    commands.resize(totalChunks);
    std::cout << chunkVertices.size() << " " << chunkIndices.size() << std::endl;
    GLCall(glBindVertexArray(VAO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, chunkVertices.size(), chunkVertices.data()));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, chunkIndices.size(), chunkIndices.data()));
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0));
    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))));
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glEnableVertexAttribArray(1));
    DrawElementsIndirectCommand cubeCommand;
    cubeCommand.count = indicesCount;
    cubeCommand.instanceCount = 1;
    cubeCommand.firstIndex = 0;
    cubeCommand.baseVertex = 0;
    cubeCommand.baseInstance = 0;
    commands.push_back(cubeCommand);
    
    currentVertexOffset += chunkVertices.size() / sizeof(GLfloat);
    currentIndexOffset += indicesCount;
    
    totalChunks++;

    offsets.push_back((int)offset);
    counts.push_back(indicesCount);
    offset += indicesCount;
}

Chunk World::GetChunk(int chunkX, int chunkY, int chunkZ) {
    return chunkHandler.chunks.find(std::make_tuple(chunkX, chunkY, chunkZ))->second;
}

World::~World() {

}