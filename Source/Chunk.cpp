#include "ChunkHandler.h"
#include "World.h"


Chunk::Chunk(int chunkX, int chunkY, int chunkZ) : blocks(blocks), chunkX(0), chunkY(0), chunkZ(0), isAllocated(false), isGenerated(false), isMeshed(false), isEmpty(false), totalMemoryUsage(0), totalBlocks(0) {

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

void Chunk::GenerateBlocks(World world, Chunk& callerChunk, bool updateCallerChunk) {
    if (isGenerated) {
        std::cerr << "[Chunk Terrain Generation / Warn] Trying to generate blocks after they had already been generated, aborting" << std::endl;
        return;
    }
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

    if (updateCallerChunk) {
        //std::cout << "[Debug] Chunk generating {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" <<  std::endl;
        //std::cout << "[Debug] Generate  caller {" << callerChunk.chunkX << ", " << callerChunk.chunkY << ", " << callerChunk.chunkZ << "}" << std::endl;
        world.GenerateChunk(callerChunk.chunkX, callerChunk.chunkY, callerChunk.chunkZ, *this, true, callerChunk);
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
            std::cerr << "[Chunk Mesh Generation / Warn] Trying to generate mesh for ungenerated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks)" << std::endl;
            return;
        }

        if (IsEmpty()) {
            std::cout << "[Chunk Mesh Generation / Info] Chunk is empty, skipping mesh generation" << std::endl;
            return;
        }

        vertices.clear();
        indices.clear();

        
        Chunk& positiveXChunk = chunkHandler.GetChunk(chunkX + 1, chunkY, chunkZ);     // Positive X
        Chunk& negativeXChunk = chunkHandler.GetChunk(chunkX - 1, chunkY, chunkZ);     // Negative X
        Chunk& positiveYChunk = chunkHandler.GetChunk(chunkX, chunkY + 1, chunkZ);     // Positive Y
        Chunk& negativeYChunk = chunkHandler.GetChunk(chunkX, chunkY - 1, chunkZ);     // Negative Y
        Chunk& positiveZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ + 1);     // Positive Z
        Chunk& negativeZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ - 1);     // Negative Z
        
        
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
                                    if (x < chunkSize - 1) {
                                        if (blocks[x + 1][y][z].GetType() == 0) {
                                            shouldAddFace = true;
                                            break;
                                        }
                                    }
                                    if (!positiveXChunk.isGenerated) {
                                        break;
                                    }
                                    if (x == chunkSize - 1) {
                                        if (positiveXChunk.blocks[0][y][z].GetType() == 0) {
                                            shouldAddFace = true;
                                            break;
                                        }
                                    }
                                    break;
                                case LEFT:
                                    if (x > 0) {
                                        if (blocks[x - 1][y][z].GetType() == 0) {
                                            shouldAddFace = true;
                                            break;
                                        }
                                    }
                                    else if (x == 0 && negativeXChunk.isGenerated) {
                                        if (negativeXChunk.blocks[chunkSize - 1][y][z].GetType() == 0) {
                                            shouldAddFace = true;
                                            break;
                                        }
                                    }
                                    else {
                                        shouldAddFace = false;
                                    }
                                    break;
                                case TOP:
                                    if (y < chunkSize - 1) {
                                        if (blocks[x][y + 1][z].GetType() == 0) {
                                            shouldAddFace = true;
                                            break;
                                        }
                                    }
                                    else if (y == chunkSize - 1 && positiveYChunk.isGenerated) {
                                        if (positiveYChunk.blocks[x][0][z].GetType() == 0) {
                                            shouldAddFace = true;
                                            break;
                                        }
                                    }
                                    else {
                                        shouldAddFace = false;
                                    }
                                    break;
                                case BOTTOM:
                                    if (y > 0) {
                                        if (blocks[x][y - 1][z].GetType() == 0) {
                                            shouldAddFace = true;
                                            break;
                                        }
                                    }
                                    else if (y == 0 && negativeYChunk.isGenerated) {
                                        if (negativeYChunk.blocks[x][chunkSize - 1][z].GetType() == 0) {
                                            shouldAddFace = true;
                                            break;
                                        }
                                    }
                                    else {
                                        shouldAddFace = false;
                                    }
                                    break;
                                case BACK:
                                    if (z < chunkSize - 1) {
                                        if (blocks[x][y][z + 1].GetType() == 0) {
                                            shouldAddFace = true;
                                            break;
                                        }
                                    }
                                    else if (z == chunkSize - 1 && positiveZChunk.isGenerated) {
                                        if (positiveZChunk.blocks[x][y][0].GetType() == 0) {
                                            shouldAddFace = true;
                                            break;
                                        }
                                    }
                                    else {
                                        shouldAddFace = false;
                                    }
                                    break;
                                case FRONT:
                                    if (z > 0) {
                                        if (blocks[x][y][z - 1].GetType() == 0) {
                                            shouldAddFace = true;
                                            break;
                                        }
                                    }
                                    else if (z == 0 && negativeZChunk.isGenerated) {
                                        if (negativeZChunk.blocks[x][y][chunkSize - 1].GetType() == 0) {
                                            shouldAddFace = true;
                                            break;
                                        }
                                    }
                                    else {
                                        shouldAddFace = false;
                                    }
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
    vertexArrayObject.LinkAttribute(vertexBufferObject, 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    vertexArrayObject.LinkAttribute(vertexBufferObject, 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoordinate));
    vertexArrayObject.LinkAttribute(vertexBufferObject, 2, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureIndex));
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
    //vertexArrayObject.Delete();
    //vertexBufferObject.Delete();
    //indexBufferObject.Delete();
}