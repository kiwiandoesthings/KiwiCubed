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
    }
    else {
        std::cerr << "[Chunk Setup / Warn] Trying to allocate chunk after it had already been allocated, aborting" << std::endl;
    }

    isAllocated = true;
}

void Chunk::GenerateBlocks() {
    if (!isGenerated) {
        if (!isAllocated) {
            std::cerr << "[Chunk Terrain Generation / Warn] Trying to generate blocks for unallocated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks)" << std::endl;
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
        //totalMemoryUsage = (float)sizeof(Chunk) + (sizeof(Block) * totalBlocks) + sizeof(vertices) + sizeof(indices);
    }
    else {
        std::cerr << "[Chunk Terrain Generation / Warn] Trying to generate blocks after they had already been generated, aborting" << std::endl;
        return;
    }

    isGenerated = true;
}

void Chunk::GenerateMesh(ChunkHandler& chunkHandler) {
    if (!isMeshed) {
        if (!isAllocated) {
            std::cerr << "[Chunk Mesh Generation / Warn] Trying to generate mesh for unallocated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks)" << std::endl;
            return;
        }

        if (!isGenerated) {
            GenerateBlocks();
        }

        if (IsEmpty()) {
            std::cout << "[Chunk Mesh Generation / Info] Chunk is empty, skipping mesh generation" << std::endl;
            return;
        }

        vertices.clear();
        indices.clear();

        Chunk* neighborChunks[6] = {
            chunkHandler.GetChunk(chunkX + 1, chunkY, chunkZ),     // Positive X
            chunkHandler.GetChunk(chunkX - 1, chunkY, chunkZ),     // Negative X
            chunkHandler.GetChunk(chunkX, chunkY + 1, chunkZ),     // Positive Y
            chunkHandler.GetChunk(chunkX, chunkY - 1, chunkZ),     // Negative Y
            chunkHandler.GetChunk(chunkX, chunkY, chunkZ + 1),     // Positive Z
            chunkHandler.GetChunk(chunkX, chunkY, chunkZ - 1)      // Negative Z
        };

        for (int x = 0; x < chunkSize; ++x) {
            for (int y = 0; y < chunkSize; ++y) {
                for (int z = 0; z < chunkSize; ++z) {
                    if (blocks[x][y][z].GetType() == 1) {
                        Block& block = blocks[x][y][z];

                        for (int direction = 0; direction < 6; ++direction) {
                            FaceDirection faceDirection = static_cast<FaceDirection>(direction);
                            bool shouldAddFace = false;
                        
                            switch (faceDirection) {
                                case RIGHT:
                                    shouldAddFace = (x == chunkSize - 1 && !neighborChunks[0]) || (x < chunkSize - 1 && blocks[x + 1][y][z].GetType() == 0);
                                    break;
                                case LEFT:
                                    shouldAddFace = (x == 0 && !neighborChunks[1]) || (x > 0 && blocks[x - 1][y][z].GetType() == 0);
                                    break;
                                case TOP:
                                    shouldAddFace = (y == chunkSize - 1 && !neighborChunks[2]) || (y < chunkSize - 1 && blocks[x][y + 1][z].GetType() == 0);
                                    break;
                                case BOTTOM:
                                    shouldAddFace = (y == 0 && !neighborChunks[3]) || (y > 0 && blocks[x][y - 1][z].GetType() == 0);
                                    break;
                                case BACK:
                                    shouldAddFace = (z == chunkSize - 1 && !neighborChunks[4]) || (z < chunkSize - 1 && blocks[x][y][z + 1].GetType() == 0);
                                    break;
                                case FRONT:
                                    shouldAddFace = (z == 0 && !neighborChunks[5]) || (z > 0 && blocks[x][y][z - 1].GetType() == 0);
                                    break;
                            }
                        
                            if (shouldAddFace) {
                                block.AddFace(vertices, indices, faceDirection, chunkX, chunkY, chunkZ, chunkSize);
                            }
                        }
                    }
                }
            }
        }
    }

    isMeshed = true;
}

void Chunk::Render() {
    if (isEmpty) {
        return;
    }
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

bool Chunk::IsEmpty() {
    if (totalBlocks > 0) {
        isEmpty = false;
        return false;
    }
    else {
        isEmpty = true;
        return true;
    }
}

Chunk::~Chunk() {
    //xvertexArrayObject.Delete();
    //vertexBufferObject.Delete();
    //indexBufferObject.Delete();
}