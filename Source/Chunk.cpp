#include <ChunkHandler.h>


Chunk::Chunk(int chunkX, int chunkY, int chunkZ) : blocks(blocks), chunkX(0), chunkY(0), chunkZ(0), isAllocated(false), isGenerated(false), isMeshed(false), isEmpty(false), totalMemoryUsage(0), totalBlocks(0)/*, startIndex(0), endIndex(0), chunkIndex(0)*/ {

}

void Chunk::AllocateChunk() {
    if (!isAllocated) {
        // Allocate memory for all the blocks in the chunk
        blocks = new Block * *[chunkSize];
        for (int i = 0; i < chunkSize; ++i) {
            blocks[i] = new Block * [chunkSize];
            for (int j = 0; j < chunkSize; ++j) {
                blocks[i][j] = new Block[chunkSize];
            }
        }

        isAllocated = true;
    }
    else {
        std::cerr << "Chunk Allocation / Warn: Trying to allocate chunk after it had already been allocated, aborting" << std::endl;
    }
}

void Chunk::GenerateBlocks() {
    if (!isGenerated) {
        if (!isAllocated) {
            std::cerr << "Chunk Block Generation / Warn: Trying to generate blocks for unallocated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks)" << std::endl;
            return;
        }

        for (int x = 0; x < chunkSize; ++x) {
            for (int y = 0; y < chunkSize; ++y) {
                for (int z = 0; z < chunkSize; ++z) {
                    blocks[x][y][z].GenerateBlock(x, y, z, chunkX, chunkY, chunkZ, chunkSize);
                    if (blocks[x][y][z].GetType() == 1) {
                        totalBlocks++;
                    }
                }
            }
        }
        totalMemoryUsage = (float)sizeof(Chunk) + (sizeof(Block) * totalBlocks) + sizeof(vertices) + sizeof(indices);
    }
    else {
        std::cerr << "Chunk Block Generation / Warn: Trying to generate blocks after they had already been generated, aborting" << std::endl;
        return;
    }

    isGenerated = true;
}

void Chunk::GenerateMesh(ChunkHandler& chunkHandler) {
    if (!isMeshed) {
        if (!isAllocated) {
            std::cerr << "Chunk Mesh Generation / Warn: Trying to generate mesh for unallocated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks)" << std::endl;
            return;
        }

        vertices.clear();
        indices.clear();

        if (!isGenerated) {
            GenerateBlocks();
        }

        if (IsArrayEmpty(blocks)) {
            isEmpty = true;
            std::cout << "Chunk Mesh Generation / Info: Chunk is empty, skipping mesh generation" << std::endl;
            return;
        }

        isEmpty = false;

        Chunk* positiveXChunk = chunkHandler.GetChunk(chunkX + 1, chunkY, chunkZ);
        Chunk* negativeXChunk = chunkHandler.GetChunk(chunkX - 1, chunkY, chunkZ);
        Chunk* positiveYChunk = chunkHandler.GetChunk(chunkX, chunkY + 1, chunkZ);
        Chunk* negativeYChunk = chunkHandler.GetChunk(chunkX, chunkY - 1, chunkZ);
        Chunk* positiveZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ + 1);
        Chunk* negativeZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ - 1);

        for (int x = 0; x < chunkSize; ++x) {
            for (int y = 0; y < chunkSize; ++y) {
                for (int z = 0; z < chunkSize; ++z) {

                    if (blocks[x][y][z].GetType() == 1) {
                        Block& block = blocks[x][y][z];

                        if (x + 1 > chunkSize - 1 && positiveXChunk == nullptr) {
                            block.AddFace(vertices, indices, RIGHT, chunkX, chunkY, chunkZ, chunkSize);
                        }
                        if (x - 1 < 0 && negativeXChunk == nullptr) {
                            block.AddFace(vertices, indices, LEFT, chunkX, chunkY, chunkZ, chunkSize);
                        }
                        if (y + 1 > chunkSize - 1 && positiveYChunk == nullptr) {
                            block.AddFace(vertices, indices, TOP, chunkX, chunkY, chunkZ, chunkSize);
                        }
                        if (y - 1 < 0 && negativeYChunk == nullptr) {
                            block.AddFace(vertices, indices, BOTTOM, chunkX, chunkY, chunkZ, chunkSize);
                        }
                        if (z + 1 > chunkSize - 1 && positiveZChunk == nullptr) {
                            block.AddFace(vertices, indices, BACK, chunkX, chunkY, chunkZ, chunkSize);
                        }
                        if (z - 1 < 0 && negativeZChunk == nullptr) {
                            block.AddFace(vertices, indices, FRONT, chunkX, chunkY, chunkZ, chunkSize);
                        }

                        // Check if the block is out of the current chunk, and the other chunk exists
                        if (x + 1 > chunkSize - 1 && positiveXChunk != nullptr) {
                            // Allocate the chunk if it isn't already
                            if (!positiveXChunk->isAllocated) {
                                positiveXChunk->AllocateChunk();
                            }
                            // Set the position, and generate the chunk if it isn't already
                            positiveXChunk->SetPosition(chunkX + 1, chunkY, chunkZ);
                            if (!positiveXChunk->isGenerated) {
                                positiveXChunk->GenerateBlocks();
                            }
                            // Allow for correct meshing between chunks
                            if (positiveXChunk->blocks[0][y][z].GetType() == 0) {
                                block.AddFace(vertices, indices, RIGHT, chunkX, chunkY, chunkZ, chunkSize);
                            }
                        }
                        // Mesh the insides of the chunk
                        else if (x + 1 <= chunkSize - 1 && blocks[x + 1][y][z].GetType() == 0) {
                            block.AddFace(vertices, indices, RIGHT, chunkX, chunkY, chunkZ, chunkSize);
                        }

                        if (x - 1 < 0 && negativeXChunk != nullptr) {
                            if (!negativeXChunk->isAllocated) {
                                negativeXChunk->AllocateChunk();
                            }
                            negativeXChunk->SetPosition(chunkX - 1, chunkY, chunkZ);
                            if (!negativeXChunk->isGenerated) {
                                negativeXChunk->GenerateBlocks();
                            }
                            if (negativeXChunk->blocks[chunkSize - 1][y][z].GetType() == 0) {
                                block.AddFace(vertices, indices, LEFT, chunkX, chunkY, chunkZ, chunkSize);
                            }
                        }
                        else if (x - 1 >= 0 && blocks[x - 1][y][z].GetType() == 0) {
                            block.AddFace(vertices, indices, LEFT, chunkX, chunkY, chunkZ, chunkSize);
                        }

                        if (y + 1 > chunkSize - 1 && positiveYChunk != nullptr) {
                            if (!positiveYChunk->isAllocated) {
                                positiveYChunk->AllocateChunk();
                            }
                            positiveYChunk->SetPosition(chunkX, chunkY + 1, chunkZ);
                            if (!positiveYChunk->isGenerated) {
                                positiveYChunk->GenerateBlocks();
                            }
                            if (positiveYChunk->blocks[x][1][z].GetType() == 0) {
                                block.AddFace(vertices, indices, TOP, chunkX, chunkY, chunkZ, chunkSize);
                            }
                        }
                        else if (y + 1 <= chunkSize - 1 && blocks[x][y + 1][z].GetType() == 0) {
                            block.AddFace(vertices, indices, TOP, chunkX, chunkY, chunkZ, chunkSize);
                        }

                        if (y - 1 < 0 && negativeYChunk != nullptr) {
                            if (!negativeYChunk->isAllocated) {
                                negativeYChunk->AllocateChunk();
                            }
                            negativeYChunk->SetPosition(chunkX, chunkY - 1, chunkZ);
                            if (!negativeYChunk->isGenerated) {
                                negativeYChunk->GenerateBlocks();
                            }
                            if (negativeYChunk->blocks[x][chunkSize - 1][z].GetType() == 0) {
                                block.AddFace(vertices, indices, BOTTOM, chunkX, chunkY, chunkZ, chunkSize);
                            }
                        }
                        else if (y - 1 >= 0 && blocks[x][y - 1][z].GetType() == 0) {
                            block.AddFace(vertices, indices, BOTTOM, chunkX, chunkY, chunkZ, chunkSize);
                        }

                        if (z + 1 > chunkSize - 1 && positiveZChunk != nullptr) {
                            if (!positiveZChunk->isAllocated) {
                                positiveZChunk->AllocateChunk();
                            }
                            positiveZChunk->SetPosition(chunkX, chunkY, chunkZ + 1);
                            if (!positiveZChunk->isGenerated) {
                                positiveZChunk->GenerateBlocks();
                            }
                            if (positiveZChunk->blocks[x][y][0].GetType() == 0) {
                                block.AddFace(vertices, indices, BACK, chunkX, chunkY, chunkZ, chunkSize);
                            }
                        }
                        else if (z + 1 <= chunkSize - 1 && blocks[x][y][z + 1].GetType() == 0) {
                            block.AddFace(vertices, indices, BACK, chunkX, chunkY, chunkZ, chunkSize);
                        }

                        if (z - 1 < 0 && negativeZChunk != nullptr) {
                            if (!negativeZChunk->isAllocated) {
                                negativeZChunk->AllocateChunk();
                            }
                            negativeZChunk->SetPosition(chunkX, chunkY, chunkZ - 1);
                            if (!negativeZChunk->isGenerated) {
                                negativeZChunk->GenerateBlocks();
                            }
                            if (negativeZChunk->blocks[x][y][chunkSize - 1].GetType() == 0) {
                                block.AddFace(vertices, indices, FRONT, chunkX, chunkY, chunkZ, chunkSize);
                            }
                        }
                        else if (z - 1 >= 0 && blocks[x][y][z - 1].GetType() == 0) {
                            block.AddFace(vertices, indices, FRONT, chunkX, chunkY, chunkZ, chunkSize);
                        }
                    }
                }
            }
        }

        //auto start_time = std::chrono::high_resolution_clock::now();

        // Generate mesh for all the blocks in a chunk by querying blocks around it and marking faces to be generated


        //auto end_time = std::chrono::high_resolution_clock::now();
        //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        //std::cout << "Chunk Generation / Info: Chunk mesh generation took " << duration << " ms" << std::endl;
    }

    isMeshed = true;
}

void Chunk::Render(Shader shaderProgram) {
    if (isEmpty) {
        return;
    }
    glUniform3f(glGetUniformLocation(shaderProgram.shaderProgramID, "chunkPosition"), (GLfloat)chunkX, (GLfloat)chunkY, (GLfloat)chunkZ);
    vertexArrayObject.Bind();
    vertexBufferObject.Bind();
    vertexBufferObject.Setup(vertices.size() * sizeof(GLfloat), vertices.data());
    indexBufferObject.Bind();
    indexBufferObject.Setup(indices.size() * sizeof(GLuint), indices.data());
    vertexArrayObject.LinkAttribute(vertexBufferObject, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    vertexArrayObject.LinkAttribute(vertexBufferObject, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    GLCall(glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0));
}

void Chunk::SetPosition(int newChunkX, int newChunkY, int newChunkZ) {
    chunkX = newChunkX;
    chunkY = newChunkY;
    chunkZ = newChunkZ;
}

int Chunk::GetTotalBlocks() {
    return totalBlocks;
}

std::vector<GLfloat> Chunk::GetVertices() {
    return vertices;
}

std::vector<GLuint> Chunk::GetIndices() {
    return indices;
}

int Chunk::GetMemoryUsage() {
    return 20 * totalBlocks;
}

void Chunk::SetStartIndex(int newStartIndex) {
    startIndex = newStartIndex;
}

int Chunk::GetStartIndex() {
    return startIndex;
}

void Chunk::SetEndIndex(int newEndIndex) {
    endIndex = newEndIndex;
}

int Chunk::GetEndIndex() {
    return endIndex;
}

void Chunk::SetChunkIndex(int newChunkIndex) {
    chunkIndex = newChunkIndex;
}

int Chunk::GetChunkIndex() {
    return chunkIndex;
}

bool Chunk::IsArrayEmpty(Block*** blocks) {
    if (totalBlocks > 0) {
        return false;
    }
    else {
        return true;
    }
}

Chunk::~Chunk() {
    //xvertexArrayObject.Delete();
    //vertexBufferObject.Delete();
    //indexBufferObject.Delete();
}