#include "ChunkHandler.h"
#include "World.h"


Chunk::Chunk(int chunkX, int chunkY, int chunkZ) : blocks(nullptr), chunkX(0), chunkY(0), chunkZ(0), isAllocated(false), isGenerated(false), isMeshed(false), isEmpty(false), totalMemoryUsage(0), totalBlocks(0)/*,vertexBufferObject(("chunk " + std::to_string(chunkX) + " " + std::to_string(chunkY) + " " + std::to_string(chunkZ)).c_str())*/ {

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
    generationStatus = 1;
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
                //std::cout << blocks[x][y][z].GetType() << std::endl;
                if (blocks[x][y][z].GetType() > 0) {
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
    generationStatus = 2;
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
                    //std::cout << blocks[x][y][z].GetType() << " e" << std::endl;
                    if (blocks[x][y][z].GetType() > 0) {
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
    generationStatus = 3;
}

void Chunk::Render() {
    if (isEmpty) {
        return;
    }
    
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

std::vector<GLfloat> Chunk::GetVertices() const {
    return vertices;
}

std::vector<GLuint> Chunk::GetIndices() const {
    return indices;
}

unsigned int Chunk::GetMemoryUsage() const {
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

void Chunk::DisplayImGui() const {
    ImGui::Text("Chunk Position: {%d, %d, %d}, GS: %d, Blocks: %d, VBO: %d", chunkX, chunkY, chunkZ, generationStatus, totalBlocks, vertexBufferObject.vertexBufferObjectID);
}

Chunk::~Chunk() {
    //vertexArrayObject.Delete();
    //vertexBufferObject.Delete();
    //indexBufferObject.Delete();
}