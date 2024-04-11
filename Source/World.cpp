#include <World.h>


World::World() : totalChunks(0), totalMemoryUsage(0) {
    for (int x = 0; x < worldSize - 1; ++x) {
        for (int y = 0; y < worldSize - 1; ++y) {
            for (int z = 0; z < worldSize - 1; ++z) {
                // Insert the postion of the chunk and a new chunk instance to the map
                sparseVoxelOctree.chunks.insert(std::make_pair(std::tuple(x, y, z), Chunk(x, y, z)));
            }
        }
    }

    //drawCount = worldSize * worldSize * worldSize;

    //GLCall(glGenBuffers(1, &indirectBufferObject));

    //GLCall(glGenBuffers(1, &chunkDataBufferObject));
    //GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunkDataBufferObject));
    //GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ChunkData) * totalChunks, chunkData.data(), GL_STATIC_DRAW));
}

void World::Render(Shader shaderProgram) {
    //renderer.DrawElements(vertexArrayObject, vertexBufferObject, indexBufferObject, vertices, indices);

    // Deprecated rendering right now
    //vertexArrayObject.Bind();
    //vertexBufferObject.Bind();
    //vertexBufferObject.Setup(vertices.size() * sizeof(GLfloat), vertices.data());
    //indexBufferObject.Bind();
    //indexBufferObject.Setup(indices.size() * sizeof(GLuint), indices.data());
    //vertexArrayObject.LinkAttribute(vertexBufferObject, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    //vertexArrayObject.LinkAttribute(vertexBufferObject, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    //GLCall(glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0));

    for (int x = 0; x < worldSize - 1; ++x) {
		for (int y = 0; y < worldSize - 1; ++y) {
			for (int z = 0; z < worldSize - 1; ++z) {
				sparseVoxelOctree.chunks.find(std::make_tuple(x, y, z))->second.Render(shaderProgram);
			}
		}
	}

    // The rest of this is a work in progress

    //drawCount = totalChunks;

    // Setup the draw indirect buffer and chunk data buffer
    //GLCall(glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferObject));
    //GLCall(glBufferData(GL_DRAW_INDIRECT_BUFFER, commands.size() * sizeof(DrawElementsIndirectCommand), commands.data(), GL_STATIC_DRAW));
    //GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunkDataBufferObject));

    // Do ALL the draw calls in ONE CALL MUAHAHAHAAHAAH
    //GLCall(glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, commands.size(), 0));
}

void World::GenerateWorld() {
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int x = 0; x < worldSize - 1; ++x) {
        for (int y = 0; y < worldSize - 1; ++y) {
            for (int z = 0; z < worldSize - 1; ++z) {
                GenerateChunk(x, y, z);
            }
        }
    }

    // Calculate the memory usage of the chunk and at it to total memory usage
    //float sizeInBytes = sizeof(World) + sizeof(vertices) + sizeof(indices);
    //float sizeInMegabytes = sizeInBytes / (1024.0f * 1024.0f);
    //totalMemoryUsage += sizeInMegabytes;

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0f;

    float chunkGenerationSpeed = static_cast<float>(duration / (worldSize * worldSize * worldSize));

    std::cout << "World Creation / Info: Total world preparation with chunk count of " << worldSize * worldSize * worldSize << " took " << duration << " ms" << " or roughly " << chunkGenerationSpeed << " ms per chunk (slightly innacurate as empty chunks are skipped) And around " << (int)1000 / chunkGenerationSpeed << " chunks generated, meshed and added per second" << std::endl;
    // Memory profiling is also super very work in progress
    //std::cout << "World Creation / Info: Memory usage of world is " << totalMemoryUsage << " megabytes" << std::endl;
}

void World::GenerateChunk(int chunkX, int chunkY, int chunkZ) {
    //auto start_time = std::chrono::high_resolution_clock::now();
    Chunk& chunk = sparseVoxelOctree.chunks.find(std::make_tuple(chunkX, chunkY, chunkZ))->second;

    // Basic procedures for preparing a chunk SetPosition should be done before ANYTHING ELSE, or functions relying on the chunks position will not work properly
    chunk.SetPosition(chunkX, chunkY, chunkZ);
    if (!chunk.isAllocated) {
        chunk.AllocateChunk();
    }
    if (!chunk.isMeshed) {
        chunk.GenerateMesh(sparseVoxelOctree);
    }

    //chunkLocations.push_back({chunkX, chunkY, chunkZ});

    //if (chunk.isEmpty) {
    //    std::cout << "World Creation / Info: Chunk is empty, so skipping appending to world mesh" << std::endl;
    //    return;
    //}

    // Get the numbers ready
    //std::vector<GLfloat> chunkVertices = chunk.GetVertices();
    //std::vector<GLuint> chunkIndices = chunk.GetIndices();

    //int startIndex = (int)vertices.size() / 3;
    //int numVertices = (int)chunkVertices.size() / 3;
    //int numIndices = (int)chunkIndices.size();

    // Add vertices and indices from the chunk
    //vertices.insert(vertices.end(), chunkVertices.begin(), chunkVertices.end());
    //indices.insert(indices.end(), chunkIndices.begin(), chunkIndices.end());

    //chunk.SetStartIndex(startIndex = (startIndex == 0) ? startIndex : startIndex + 1);
    //chunk.SetEndIndex(startIndex + numVertices);
    //
    //// We love doing magic here. Here's some magic that makes the glMultiDrawElementsIndirect function work
    //chunk.SetChunkIndex(totalChunks);
    //
    //size_t indicesCount = chunk.GetIndices().size() / sizeof(GLuint);
    //
    //commands.resize(totalChunks);
    //
    //commands.push_back(DrawElementsIndirectCommand());
    //
    //// Set up the draw call with a little bit more magic
    //commands[totalChunks].count = (GLuint)indicesCount;
    //commands[totalChunks].instanceCount = 1; // Not using instancing
    //commands[totalChunks].firstIndex = static_cast<GLuint>(currentIndexOffset * sizeof(GL_UNSIGNED_INT));
    //commands[totalChunks].baseVertex = static_cast<GLuint>(currentVertexOffset);
    //commands[totalChunks].baseInstance = glm::vec3(0, 0, 0); // Use this unused thing for chunk position
    //
    //currentVertexOffset += chunk.GetVertices().size() / sizeof(GLfloat);
    //currentIndexOffset += indicesCount;
    //
    //totalChunks++;
    //
    //// Calculate the memory usage of the chunk and at it to total memory usage
    //float sizeInBytes = chunk.totalMemoryUsage;
    //float sizeInMegabytes = sizeInBytes / (1024.0f * 1024.0f);
    //totalMemoryUsage += sizeInMegabytes;

    //auto end_time = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    //std::cout << "World Creation / Info: Appending chunk mesh to world mesh took, " << duration << " ms" << std::endl;
    //std::cout << "World Creation / Info: Chunk start index set to, " << chunk.GetStartIndex() << " and end index set to, " << chunk.GetEndIndex() << std::endl;
}

Chunk World::GetChunk(int chunkX, int chunkY, int chunkZ) {
    return sparseVoxelOctree.chunks.find(std::make_tuple(chunkX, chunkY, chunkZ))->second;
}

World::~World() {

}