#include "ChunkHandler.h"
#include "World.h"
#include <klogger.hpp>

// Currently just sets up the VBO, VAO, and IBO
void Chunk::SetupRenderComponents() {
    OVERRIDE_LOG_NAME("Chunk Render Components Setup");
    if (renderComponentsSetup) {
        ERR("Tried to setup render components when they were already setup, aborting");
    }
    vertexBufferObject.SetupBuffer();
    vertexArrayObject.SetupArrayObject();
    indexBufferObject.SetupBuffer();
    renderComponentsSetup = true;
}


void Chunk::AllocateChunk() {
    if (!isAllocated) {
        debugVisualizationVertices = {
    	     -1.0f * debugVertexScale, -1.0f * debugVertexScale,  0.0f * debugVertexScale,  0.0f,  0.0f,
    	      1.0f * debugVertexScale, -1.0f * debugVertexScale,  0.0f * debugVertexScale,  1.0f,  0.0f,
    	     1.0f * debugVertexScale, 1.0f * debugVertexScale, 0.0f * debugVertexScale, 1.0f, 1.0f,
    	    -1.0f * debugVertexScale, 1.0f * debugVertexScale, 0.0f * debugVertexScale, 0.0f, 1.0f
	    };

	    debugVisualizationIndices = {
        	0, 1, 2,
        	0, 2, 3
	    };

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
        WARN("Trying to generate blocks for unallocated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks) {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}");
        return;
    }

    for (int blockX = 0; blockX < chunkSize; ++blockX) {
        for (int blockY = 0; blockY < chunkSize; ++blockY) {
            for (int blockZ = 0; blockZ < chunkSize; ++blockZ) {
                blocks[blockX][blockY][blockZ].GenerateBlock(blockX, blockY, blockZ, chunkX, chunkY, chunkZ, chunkSize, debug);
                if (blocks[blockX][blockY][blockZ].GetBlockID() != 0) {
                    totalBlocks++;
                }
            }
        }
    }


    if (updateCallerChunk) {
        world.GenerateChunk(callerChunk.chunkX, callerChunk.chunkY, callerChunk.chunkZ, *this, true, callerChunk);
    }

    IsEmpty();
    IsFull();
    isGenerated = true;
    generationStatus = 2;
}

// Returns whether or not the mesh was generated
bool Chunk::GenerateMesh(ChunkHandler& chunkHandler, const bool remesh) {
    OVERRIDE_LOG_NAME("Chunk Mesh Generation");
    if (!(!isMeshed || remesh)) {
        WARN("Trying to generate mesh for already meshed chunk, aborting");
        return false;
    }

    if (!isAllocated) {
        ERR("Trying to generate mesh for unallocated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks) {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}");
        return false;
    }
    
    if (!isGenerated) {
        WARN("Trying to generate mesh for ungenerated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks) {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}");
        return false;
    }

    if (IsEmpty() || IsFull()) {
        INFO("Chunk is empty or full, skipping mesh generation {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}");
        return false;
    }

    vertices.clear();
    indices.clear();
    
    Chunk& positiveXChunk = chunkHandler.GetChunk(chunkX + 1, chunkY, chunkZ, true);     // Positive X
    Chunk& negativeXChunk = chunkHandler.GetChunk(chunkX - 1, chunkY, chunkZ, true);     // Negative X
    Chunk& positiveYChunk = chunkHandler.GetChunk(chunkX, chunkY + 1, chunkZ, true);     // Positive Y
    Chunk& negativeYChunk = chunkHandler.GetChunk(chunkX, chunkY - 1, chunkZ, true);     // Negative Y
    Chunk& positiveZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ + 1, true);     // Positive Z
    Chunk& negativeZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ - 1, true);     // Negative Z

    for (int x = 0; x < chunkSize; ++x) {
        for (int y = 0; y < chunkSize; ++y) {
            for (int z = 0; z < chunkSize; ++z) {
                if (blocks[x][y][z].GetBlockID() != 0) {
                    Block& block = blocks[x][y][z];
                    
                    for (int direction = 0; direction < 6; ++direction) {
                        FaceDirection faceDirection = static_cast<FaceDirection>(direction);
                        bool shouldAddFace = false;
                        switch (faceDirection) {
                            case RIGHT:
                                if (x < chunkSize - 1) {
                                    if (blocks[x + 1][y][z].GetBlockID() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                if (!positiveXChunk.isGenerated) {
                                    break;
                                }
                                if (x == chunkSize - 1) {
                                    if (positiveXChunk.blocks[0][y][z].GetBlockID() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                break;
                            case LEFT:
                                if (x > 0) {
                                    if (blocks[x - 1][y][z].GetBlockID() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                else if (x == 0 && negativeXChunk.isGenerated) {
                                    if (negativeXChunk.blocks[chunkSize - 1][y][z].GetBlockID() == 0) {
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
                                    if (blocks[x][y + 1][z].GetBlockID() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                else if (y == chunkSize - 1 && positiveYChunk.isGenerated) {
                                    if (positiveYChunk.blocks[x][0][z].GetBlockID() == 0) {
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
                                    if (blocks[x][y - 1][z].GetBlockID() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                else if (y == 0 && negativeYChunk.isGenerated) {
                                    if (negativeYChunk.blocks[x][chunkSize - 1][z].GetBlockID() == 0) {
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
                                    if (blocks[x][y][z + 1].GetBlockID() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                else if (z == chunkSize - 1 && positiveZChunk.isGenerated) {
                                    if (positiveZChunk.blocks[x][y][0].GetBlockID() == 0) {
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
                                    if (blocks[x][y][z - 1].GetBlockID() == 0) {
                                        shouldAddFace = true;
                                        break;
                                    }
                                }
                                else if (z == 0 && negativeZChunk.isGenerated) {
                                    if (negativeZChunk.blocks[x][y][chunkSize - 1].GetBlockID() == 0) {
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

    isMeshed = true;
    generationStatus = 3;

    return true;
}

void Chunk::Render() {
    if (shouldRender) {
        if (!renderComponentsSetup) {
            SetupRenderComponents();
        }
        renderer.DrawElements(vertexArrayObject, vertexBufferObject, indexBufferObject, vertices, indices);
    }
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

bool Chunk::GetMeshable(ChunkHandler& chunkHandler) const {
    if (!isAllocated || !isGenerated || isEmpty || isFull) {
        return false;
    } else {
        Chunk& positiveXChunk = chunkHandler.GetChunk(chunkX + 1, chunkY, chunkZ, false);
        Chunk& negativeXChunk = chunkHandler.GetChunk(chunkX - 1, chunkY, chunkZ, false);
        Chunk& positiveYChunk = chunkHandler.GetChunk(chunkX, chunkY + 1, chunkZ, false);
        Chunk& negativeYChunk = chunkHandler.GetChunk(chunkX, chunkY - 1, chunkZ, false);
        Chunk& positiveZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ + 1, false);
        Chunk& negativeZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ - 1, false);

        return positiveXChunk.isGenerated && negativeXChunk.isGenerated && positiveYChunk.isGenerated && negativeYChunk.isGenerated && positiveZChunk.isGenerated && negativeZChunk.isGenerated;
    }
}

std::vector<GLfloat>& Chunk::GetVertices() {
    return vertices;
}

std::vector<GLuint>& Chunk::GetIndices() {
    return indices;
}

std::vector<GLfloat>& Chunk::GetDebugVisualizationVertices() {
    return debugVisualizationVertices;
}

std::vector<GLuint>& Chunk::GetDebugVisualizationIndices() {
    return debugVisualizationIndices;
}

unsigned int Chunk::GetMemoryUsage() {
    totalMemoryUsage = (sizeof(Block) * totalBlocks) + (sizeof(Vertex) * vertices.size()) + (sizeof(GLuint) * indices.size()) + sizeof(Chunk);
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

void Chunk::DisplayImGui() {
    ImGui::Text("Chunk position: {%d, %d, %d}, GS: %d, Blocks: %d, Vertices: %d, Indices: %d, VBO: %d, VAO: %d, IBO %d, Memory usage: %d KB", chunkX, chunkY, chunkZ, generationStatus, totalBlocks, static_cast<int>(vertices.size()), static_cast<int>(indices.size()), vertexBufferObject.vertexBufferObjectID, vertexArrayObject.vertexArrayObjectID, indexBufferObject.indexBufferObjectID, static_cast<int>(static_cast<float>((sizeof(Block) * totalBlocks) + (sizeof(Vertex) * vertices.size()) + (sizeof(GLuint) * indices.size())) / 1024.0));
}

void Chunk::Delete() {
    vertexArrayObject.Delete();
    vertexBufferObject.Delete();
    indexBufferObject.Delete();

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

    debugVisualizationVertices.clear();
    debugVisualizationIndices.clear();

}