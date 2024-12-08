#include "ChunkHandler.h"
#include "World.h"


Chunk::Chunk(int chunkX, int chunkY, int chunkZ) : blocks(nullptr), chunkX(chunkX), chunkY(chunkY), chunkZ(chunkZ), isAllocated(false), isGenerated(false), isMeshed(false), isEmpty(false), isFull(false), totalMemoryUsage(0), totalBlocks(0)/*,vertexBufferObject(("chunk " + std::to_string(chunkX) + " " + std::to_string(chunkY) + " " + std::to_string(chunkZ)).c_str())*/ {
}

// Currently just sets up the VBO, VAO, and IBO
void Chunk::SetupRenderComponents() {
    vertexBufferObject.SetupBuffer();
    vertexArrayObject.SetupArrayObject();
    indexBufferObject.SetupBuffer();
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
        //std::cerr << "[Chunk Setup / Warn] Trying to allocate chunk after it had already been allocated, aborting {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
    }

    isAllocated = true;
    generationStatus = 1;
}

void Chunk::GenerateBlocks(World& world, Chunk& callerChunk, bool updateCallerChunk, bool debug) {
    if (isGenerated) {
        //std::cerr << "[Chunk Terrain Generation / Warn] Trying to generate blocks after they had already been generated, aborting {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
        return;
    }
    if (!isAllocated) {
        std::cerr << "[Chunk Terrain Generation / Warn] Trying to generate blocks for unallocated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks) {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
        return;
    }

    if (!debug) {
        for (int x = 0; x < chunkSize; ++x) {
            for (int y = 0; y < chunkSize; ++y) {
                for (int z = 0; z < chunkSize; ++z) {
                    blocks[x][y][z].GenerateBlock(x, y, z, chunkX, chunkY, chunkZ, chunkSize, false);
                    if (blocks[x][y][z].GetType() > 0) {
                        totalBlocks++;
                    }
                }
            }
        }
    }

    if (debug) {
        for (int x = 0; x < chunkSize; ++x) {
            for (int y = 0; y < chunkSize; ++y) {
                for (int z = 0; z < chunkSize; ++z) {
                    blocks[x][y][z].GenerateBlock(x, y, z, chunkX, chunkY, chunkZ, chunkSize, true);
                    if (blocks[x][y][z].GetType() > 0) {
                        totalBlocks++;
                    }
                }
            }
        }
    }

    if (updateCallerChunk) {
        world.GenerateChunk(callerChunk.chunkX, callerChunk.chunkY, callerChunk.chunkZ, *this, true, callerChunk);
    }

    isGenerated = true;
    generationStatus = 2;
}

void Chunk::GenerateMesh(ChunkHandler& chunkHandler, const bool remesh) {
    if (!isMeshed || remesh) {
        if (!isAllocated) {
            std::cerr << "[Chunk Mesh Generation / Warn] Trying to generate mesh for unallocated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks) {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
            return;
        }

        if (!isGenerated) {
            std::cerr << "[Chunk Mesh Generation / Warn] Trying to generate mesh for ungenerated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks) {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
            return;
        }

        if (IsEmpty()) {
            std::cout << "[Chunk Mesh Generation / Info] Chunk is empty, skipping mesh generation {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
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

        for (int blockX = 0; blockX < chunkSize; ++blockX) {
            for (int blockY = 0; blockY < chunkSize; ++blockY) {
                for (int blockZ = 0; blockZ < chunkSize; ++blockZ) {
                    if (blocks[blockX][blockY][blockZ].GetType() > 0) {
                        Block& block = blocks[blockX][blockY][blockZ];

                        for (int direction = 0; direction < 6; ++direction) {
                            FaceDirection faceDirection = static_cast<FaceDirection>(direction);
                            bool shouldAddFace = false;

                            switch (faceDirection) {
                            case RIGHT:
                                if (blockX < chunkSize - 1) {
                                    if (blocks[blockX + 1][blockY][blockZ].GetType() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                if (!positiveXChunk.isGenerated) {
                                    break;
                                }
                                if (blockX == chunkSize - 1) {
                                    if (positiveXChunk.blocks[0][blockY][blockZ].GetType() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                break;
                            case LEFT:
                                if (blockX > 0) {
                                    if (blocks[blockX - 1][blockY][blockZ].GetType() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                else if (blockX == 0 && negativeXChunk.isGenerated) {
                                    if (negativeXChunk.blocks[chunkSize - 1][blockY][blockZ].GetType() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                else {
                                    shouldAddFace = false;
                                }
                                break;
                            case TOP:
                                if (blockY < chunkSize - 1) {
                                    if (blocks[blockX][blockY + 1][blockZ].GetType() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                else if (blockY == chunkSize - 1 && positiveYChunk.isGenerated) {
                                    if (positiveYChunk.blocks[blockX][0][blockZ].GetType() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                else {
                                    shouldAddFace = false;
                                }
                                break;
                            case BOTTOM:
                                if (blockY > 0) {
                                    if (blocks[blockX][blockY - 1][blockZ].GetType() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                else if (blockY == 0 && negativeYChunk.isGenerated) {
                                    if (negativeYChunk.blocks[blockX][chunkSize - 1][blockZ].GetType() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                else {
                                    shouldAddFace = false;
                                }
                                break;
                            case BACK:
                                if (blockZ < chunkSize - 1) {
                                    if (blocks[blockX][blockY][blockZ + 1].GetType() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                else if (blockZ == chunkSize - 1 && positiveZChunk.isGenerated) {
                                    if (positiveZChunk.blocks[blockX][blockY][0].GetType() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                else {
                                    shouldAddFace = false;
                                }
                                break;
                            case FRONT:
                                if (blockZ > 0) {
                                    if (blocks[blockX][blockY][blockZ - 1].GetType() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                else if (blockZ == 0 && negativeZChunk.isGenerated) {
                                    if (negativeZChunk.blocks[blockX][blockY][chunkSize - 1].GetType() == 0) {
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
                                block.AddFace(vertices, indices, faceDirection, chunkX, chunkY, chunkZ, blockX, blockY, blockZ, chunkSize);
                            }
                        }
                    }
                }
            }
        }
    }

    isMeshed = true;
    generationStatus = 3;
    IsEmpty();
}

void Chunk::Render() {
    renderer.DrawElements(vertexArrayObject, vertexBufferObject, indexBufferObject, vertices, indices);
}

void Chunk::SetPosition(int newChunkX, int newChunkY, int newChunkZ) {
    chunkX = newChunkX;
    chunkY = newChunkY;
    chunkZ = newChunkZ;
}

int Chunk::GetTotalBlocks() const {
    return totalBlocks;
}

void Chunk::SetTotalBlocks(unsigned short newTotalBlocks) {
    totalBlocks = newTotalBlocks;
}

std::vector<GLfloat> Chunk::GetVertices() const {
    return vertices;
}

std::vector<GLuint> Chunk::GetIndices() const {
    return indices;
}

unsigned int Chunk::GetMemoryUsage() {
    totalMemoryUsage = (sizeof(Block) * totalBlocks) + (sizeof(Vertex) * vertices.size()) + (sizeof(GLuint) * indices.size());
    return totalMemoryUsage;
}

bool Chunk::IsEmpty() {
    if (totalBlocks != 0) {
        isEmpty = false;
        return false;
    }
    else {
        isEmpty = true;
        return true;
    }
}

bool Chunk::IsFull() {
    if (totalBlocks == chunkSize * chunkSize * chunkSize) {
        isFull = true;
        return true;
    }
    else {
        isFull = false;
        return false;
    }
}

void Chunk::DisplayImGui() const {
    ImGui::Text("Chunk position: {%d, %d, %d}, GS: %d, Blocks: %d, Vertices: %d, Indices: %d, VBO: %d, VAO: %d, IBO %d, Memory usage: %d KB", chunkX, chunkY, chunkZ, generationStatus, totalBlocks, vertices.size(), indices.size(), vertexBufferObject.vertexBufferObjectID, vertexArrayObject.vertexArrayObjectID, indexBufferObject.indexBufferObjectID, static_cast<int>(static_cast<float>((sizeof(Block) * totalBlocks) + (sizeof(Vertex) * vertices.size()) + (sizeof(GLuint) * indices.size())) / 1024.0));
}

void Chunk::Delete() {
    //vertexArrayObject.Delete();
    //vertexBufferObject.Delete();
    //indexBufferObject.Delete();

    if (blocks) {
        for (int x = 0; x < chunkSize; ++x) {
            for (int y = 0; y < chunkSize; ++y) {
                delete[] blocks[x][y];
            }
            delete[] blocks[x];
        }
        delete[] blocks;
        blocks = nullptr;
    }

    isAllocated = false;
    isGenerated = false;
    isMeshed = false;
    generationStatus = 0;
    isEmpty = true;
    isFull = false;
    totalBlocks = 0;

    vertices.clear();
    indices.clear();
}