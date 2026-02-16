#include "ChunkHandler.h"
#include "World.h"
#include <chrono>
#include <klogger.hpp>


unsigned int Chunk::totalChunks = 0;


Chunk::Chunk(int chunkX, int chunkY, int chunkZ, ChunkHandler& chunkHandler) : chunkX(chunkX), chunkY(chunkY), chunkZ(chunkZ), chunkHandler(chunkHandler) {
    totalChunks++;
}

bool Chunk::IsReal() {
    return isRealChunk;
}

unsigned char Chunk::GetGenerationStatus() {
	if (isMeshed) {
		return 3;
	} else if (isGenerated) {
		return 2;
	} else if (isAllocated) {
		return 1;
	} else {
		return 0;
	}
}

// Currently just sets up the VBO, VAO, and IBO
void Chunk::SetupRenderComponents() {
    OVERRIDE_LOG_NAME("Chunk Render Components Setup");
    if (renderComponentsSetup) {
        WARN("Tried to setup render components when they were already setup, aborting");
    }
    vertexBufferObject.SetupBuffer();
    vertexArrayObject.SetupArrayObject();
    indexBufferObject.SetupBuffer();
    renderComponentsSetup = true;
}

// This function should be removed sometime in favor of a setup function, now that we don't actually really allocate anything
void Chunk::AllocateChunk() {
    if (isAllocated) {
        //std::cerr << "[Chunk Setup / Warn] Trying to allocate chunk after it had already been allocated, aborting {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
    }

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

    isAllocated = true;
}

bool Chunk::GenerateBlocks(World& world, Chunk* callerChunk, bool updateCallerChunk, bool debug) {
    OVERRIDE_LOG_NAME("Chunk Block Generation");
    if (isGenerated) {
        //CRITICAL("Trying to generate blocks after they had already been generated, aborting. (This should never happen, report a bug if you encounter this, thanks) {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}");
        //psnip_trap();
    }
    if (!isAllocated) {
        CRITICAL("Trying to generate blocks for unallocated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks) {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}");
        psnip_trap();
    }

    std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();

    FastNoiseLite& noise = world.GetNoise();

    std::unordered_map<AssetStringID, unsigned short> blockIDCache;
    std::unordered_map<unsigned int, unsigned short> blockVariantCountCache;

    auto GetCachedID = [&](const AssetStringID& assetStringID) -> unsigned short {
        auto iterator = blockIDCache.find(assetStringID);
        if (iterator != blockIDCache.end()) {
            return iterator->second;
        }

        unsigned short numericalID = static_cast<unsigned short>(*BlockManager::GetInstance().GetNumericalID(assetStringID));
        blockIDCache[assetStringID] = numericalID;
        return numericalID;
    };

    auto GetCachedVariantCount = [&](const unsigned int blockNumericalID) -> unsigned short {
        auto iterator = blockVariantCountCache.find(blockNumericalID);
        if (iterator != blockVariantCountCache.end()) {
            return iterator->second;
        }

        unsigned short variantCount = static_cast<unsigned short>(assetManager.GetTextureAtlasData(blockNumericalID)->size());
        blockVariantCountCache[blockNumericalID] = variantCount;
        return variantCount;
    };

    for (int blockX = 0; blockX < chunkSize; ++blockX) {
        for (int blockY = 0; blockY < chunkSize; ++blockY) {
            for (int blockZ = 0; blockZ < chunkSize; ++blockZ) {
                Block& block = GetBlock(blockX, blockY, blockZ);
                float density = noise.GetNoise(static_cast<float>(blockX + (chunkX * chunkSize)), static_cast<float>(blockZ + (chunkZ * chunkSize)));
	            int height = blockY + (chunkY * chunkSize);
                int reach = static_cast<int>(density * 16) + 30;

                if (!(height < reach)) {
                    block.blockID = 0;
		            block.variant = 0;
                    continue;
                }

                if (height + 4 < reach) {
                    block.blockID = GetCachedID(AssetStringID{"kiwicubed", "block/stone"});
		            block.variant = rand() % GetCachedVariantCount(block.blockID);
                } else if (height + 1 < reach) {
                    block.blockID = GetCachedID(AssetStringID{"kiwicubed", "block/dirt"});
		            block.variant = 0;
                } else {
                    block.blockID = GetCachedID(AssetStringID{"kiwicubed", "block/grass"});
		            block.variant = 0;
                }
                totalBlocks++;
            }
        }
    }


    if (updateCallerChunk) {
        world.GenerateChunk(callerChunk->chunkX, callerChunk->chunkY, callerChunk->chunkZ, this, true, callerChunk);
    }

    isGenerated = true;
    IsEmpty();
    IsFull();
    GenerateHeightmap();

    std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
    int time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    if (Globals::GetInstance().debugMode) {
        //INFO("Chunk generation took " + std::to_string(time) + "us");
    }

    return true;
}

// Returns whether or not the mesh was generated
bool Chunk::GenerateMesh(const bool remesh) {
    OVERRIDE_LOG_NAME("Chunk Mesh Generation");
    if (isMeshed && !remesh) {
        WARN("Trying to generate mesh for already meshed chunk, aborting");
        return false;
    }

    if (!isAllocated) {
        CRITICAL("Trying to generate mesh for unallocated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks) {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}");
        psnip_trap();
        return false;
    }
    
    if (!isGenerated) {
        WARN("Trying to generate mesh for ungenerated chunk, aborting. {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}");
        return false;
    }
    if (IsEmpty()) {
        INFO("Chunk is empty, skipping mesh generation {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}");
        return false;
    }

    std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();

    vertices.clear();
    indices.clear();
    Chunk* positiveXChunk = chunkHandler.GetChunkUnlocked(chunkX + 1, chunkY, chunkZ, true);
    Chunk* negativeXChunk = chunkHandler.GetChunkUnlocked(chunkX - 1, chunkY, chunkZ, true);
    Chunk* positiveYChunk = chunkHandler.GetChunkUnlocked(chunkX, chunkY + 1, chunkZ, true);
    Chunk* negativeYChunk = chunkHandler.GetChunkUnlocked(chunkX, chunkY - 1, chunkZ, true);
    Chunk* positiveZChunk = chunkHandler.GetChunkUnlocked(chunkX, chunkY, chunkZ + 1, true);
    Chunk* negativeZChunk = chunkHandler.GetChunkUnlocked(chunkX, chunkY, chunkZ - 1, true);

    std::vector<FaceDirection> facesToAdd;
    facesToAdd.reserve(6);

    robin_hood::unordered_flat_map<unsigned char, BlockType> textureAtlasDataCache;

    auto GetCachedBlockType = [&](const unsigned char numericalID) -> const BlockType& {
        auto iterator = textureAtlasDataCache.find(numericalID);
        if (iterator != textureAtlasDataCache.end()) {
            return iterator->second;
        }

        BlockType blockType = *BlockManager::GetInstance().GetBlockType(numericalID);
        auto [newIterator, inserted] = textureAtlasDataCache.emplace(numericalID, std::move(blockType));
        return newIterator->second;
    };

    for (int blockX = 0; blockX < chunkSize; ++blockX) {
        for (int blockY = 0; blockY < chunkSize; ++blockY) {
            for (int blockZ = 0; blockZ < chunkSize; ++blockZ) {
                if (!GetBlock(blockX, blockY, blockZ).IsAir()) {
                    Block& block = GetBlock(blockX, blockY, blockZ);

                    facesToAdd.clear();

                    const BlockType& blockType = GetCachedBlockType(block.GetBlockID());
                    int variant = block.GetVariant();
                    
                    for (int direction = 0; direction < 6; ++direction) {
                        FaceDirection faceDirection = static_cast<FaceDirection>(direction);
                        switch (faceDirection) {
                            case RIGHT:
                                if (blockX < chunkSize - 1) {
                                    if (GetBlock(blockX + 1, blockY, blockZ).IsAir()) {
                                        facesToAdd.emplace_back(RIGHT);
                                        break;
                                    }
                                }
                                else if (blockX == chunkSize - 1 && positiveXChunk->isGenerated) {
                                    if (positiveXChunk->GetBlock(0, blockY, blockZ).IsAir()) {
                                        facesToAdd.emplace_back(RIGHT);
                                        break;
                                    }
                                }
                                break;
                            case LEFT:
                                if (blockX > 0) {
                                    if (GetBlock(blockX - 1, blockY, blockZ).IsAir()) {
                                        facesToAdd.emplace_back(LEFT);
                                        break;
                                    }
                                }
                                else if (blockX == 0 && negativeXChunk->isGenerated) {
                                    if (negativeXChunk->GetBlock(chunkSize - 1, blockY, blockZ).IsAir()) {
                                        facesToAdd.emplace_back(LEFT);
                                        break;
                                    }
                                }
                                break;
                            case TOP:
                                if (blockY < chunkSize - 1) {
                                    if (GetBlock(blockX, blockY + 1, blockZ).IsAir()) {
                                        facesToAdd.emplace_back(TOP);
                                        break;
                                    }
                                }
                                else if (blockY == chunkSize - 1 && positiveYChunk->isGenerated) {
                                    if (positiveYChunk->GetBlock(blockX, 0, blockZ).IsAir()) {
                                        facesToAdd.emplace_back(TOP);
                                        break;
                                    }
                                }
                                break;
                            case BOTTOM:
                                if (blockY > 0) {
                                    if (GetBlock(blockX, blockY - 1, blockZ).IsAir()) {
                                        facesToAdd.emplace_back(BOTTOM);
                                        break;
                                    }
                                }
                                else if (blockY == 0 && negativeYChunk->isGenerated) {
                                    if (negativeYChunk->GetBlock(blockX, chunkSize - 1, blockZ).IsAir()) {
                                        facesToAdd.emplace_back(BOTTOM);
                                        break;
                                    }
                                }
                                break;
                            case BACK:
                                if (blockZ < chunkSize - 1) {
                                    if (GetBlock(blockX, blockY, blockZ + 1).IsAir()) {
                                        facesToAdd.emplace_back(BACK);
                                        break;
                                    }
                                }
                                else if (blockZ == chunkSize - 1 && positiveZChunk->isGenerated) {
                                    if (positiveZChunk->GetBlock(blockX, blockY, 0).IsAir()) {
                                        facesToAdd.emplace_back(BACK);
                                        break;
                                    }
                                }
                                break;
                            case FRONT:
                                if (blockZ > 0) {
                                    if (GetBlock(blockX, blockY, blockZ - 1).IsAir()) {
                                        facesToAdd.emplace_back(FRONT);
                                        break;
                                    }
                                }
                                else if (blockZ == 0 && negativeZChunk->isGenerated) {
                                    if (negativeZChunk->GetBlock(blockX, blockY, chunkSize - 1).IsAir()) {
                                        facesToAdd.emplace_back(FRONT);
                                        break;
                                    }
                                }
                                break;
                        }
                    }

                    for (unsigned int iterator = 0; iterator < facesToAdd.size(); ++iterator) {
                        const std::vector<TextureAtlasData>& atlasDatas = blockType.metaTextures[facesToAdd[iterator]].atlasData;
                        int usableVariant = variant;
                        if (variant >= atlasDatas.size()) {
                            usableVariant = variant % atlasDatas.size();
                        }
                        const TextureAtlasData& atlasData = atlasDatas[usableVariant];
		                GLuint vertexOffset = static_cast<GLuint>(facesToAdd.at(iterator)) * 20;
		                GLuint baseIndex = static_cast<GLuint>(vertices.size() / 5);

		                for (size_t i = vertexOffset; i < vertexOffset + 20; i += 5) {
		                	vertices.emplace_back((BlockModel::faceVertices[i + 0]) + static_cast<GLfloat>(blockX + (chunkX * static_cast<int>(chunkSize))));
		                	vertices.emplace_back((BlockModel::faceVertices[i + 1]) + static_cast<GLfloat>(blockY + (chunkY * static_cast<int>(chunkSize))));
		                	vertices.emplace_back((BlockModel::faceVertices[i + 2]) + static_cast<GLfloat>(blockZ + (chunkZ * static_cast<int>(chunkSize))));
                
		                	switch ((i - vertexOffset) / 5 % 4) {
		                		case 0: {
		                			float u0 = atlasData.xPosition / 4.0f; 
		                			float v1 = atlasData.yPosition / 4.0f; 
		                			vertices.emplace_back(u0);
		                			vertices.emplace_back(v1);
		                			break;
		                		}
		                		case 1: {
		                			float u1 = (atlasData.xPosition + atlasData.xSize) / 4.0f; 
		                			float v1 = atlasData.yPosition / 4.0f; 
		                			vertices.emplace_back(u1);
		                			vertices.emplace_back(v1);
		                			break;
		                		}
		                		case 2: {
		                			float u1 = (atlasData.xPosition + atlasData.xSize) / 4.0f; 
		                			float v0 = (atlasData.yPosition + atlasData.ySize) / 4.0f; 
		                			vertices.emplace_back(u1);
		                			vertices.emplace_back(v0);
		                			break;
		                		}
		                		case 3: {
		                			float u0 = atlasData.xPosition / 4.0f; 
		                			float v0 = (atlasData.yPosition + atlasData.ySize) / 4.0f;
		                			vertices.emplace_back(u0);
		                			vertices.emplace_back(v0);
		                			break;
		                		}
		                	}
		                }
                    
		                for (size_t i = 0; i < 6; ++i) {
		                	indices.emplace_back(static_cast<GLuint>(baseIndex + BlockModel::faceIndices[i]));
		                }
	                }               
                }
            }
        }
    }

    isMeshed = true;
    shouldRender = true;

    std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
    int time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    if (Globals::GetInstance().debugMode) {
        DEBUG("Chunk meshing took " + std::to_string(time) + "us", Globals::GetInstance().debugMode);
    }

    return true;
}

void Chunk::GenerateHeightmap() {
    OVERRIDE_LOG_NAME("Chunk Heightmap Generation");
    if (!isGenerated) {
        WARN("Tried to generate heightmap for ungenerated chunk {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}");
    }

    for (int blockX = 0; blockX < chunkSize; blockX++) {
        for (int blockZ = 0; blockZ < chunkSize; blockZ++) {
            bool foundLevel = false;
            for (int blockY = chunkSize - 1; blockY >= 0 && foundLevel == false; blockY--) {
                if (!GetBlock(blockX, blockY, blockZ).IsAir()) {
                    if (blockY == chunkSize - 1) {
                        heightmap.heightmap[blockX][blockZ] = 0;
                        heightmap.heightmapMask[blockX][blockZ] = true;
                    } else {
                        heightmap.heightmap[blockX][blockZ] = blockY + 1;
                        heightmap.heightmapMask[blockX][blockZ] = false;
                    }

                    foundLevel = true;
                }
            }

            if (!foundLevel) {
                heightmap.heightmap[blockX][blockZ] = 0;
                heightmap.heightmapMask[blockX][blockZ] = false;
            }
        }
    }
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

    isRealChunk = true;
}

int Chunk::GetTotalBlocks() const {
    return totalBlocks;
}

void Chunk::SetTotalBlocks(unsigned short newTotalBlocks) {
    totalBlocks = newTotalBlocks;
}

bool Chunk::GetMeshable(ChunkHandler& chunkHandler) const {
    if (isMeshed || !isAllocated || !isGenerated || isEmpty) {
        return false;
    } else {
        Chunk* positiveXChunk = chunkHandler.GetChunk(chunkX + 1, chunkY, chunkZ, false);
        Chunk* negativeXChunk = chunkHandler.GetChunk(chunkX - 1, chunkY, chunkZ, false);
        Chunk* positiveYChunk = chunkHandler.GetChunk(chunkX, chunkY + 1, chunkZ, false);
        Chunk* negativeYChunk = chunkHandler.GetChunk(chunkX, chunkY - 1, chunkZ, false);
        Chunk* positiveZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ + 1, false);
        Chunk* negativeZChunk = chunkHandler.GetChunk(chunkX, chunkY, chunkZ - 1, false);

        return positiveXChunk->isGenerated && negativeXChunk->isGenerated && positiveYChunk->isGenerated && negativeYChunk->isGenerated && positiveZChunk->isGenerated && negativeZChunk->isGenerated;
    }
}

int Chunk::GetHeightmapLevelAt(glm::ivec2 position) {
    if (!isGenerated) {
        return -1;
    }

    if (heightmap.heightmapMask[position.x][position.y]) {
        return 0;
    } else {
        return heightmap.heightmap[position.x][position.y];
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
    ImGui::Text("Chunk position: {%d, %d, %d}, GS: %d, Blocks: %d, Vertices: %d, Indices: %d, VBO: %d, VAO: %d, IBO %d, Memory usage: %d KB", chunkX, chunkY, chunkZ, GetGenerationStatus(), totalBlocks, static_cast<int>(vertices.size()), static_cast<int>(indices.size()), vertexBufferObject.vertexBufferObjectID, vertexArrayObject.vertexArrayObjectID, indexBufferObject.indexBufferObjectID, static_cast<int>(static_cast<float>((sizeof(Block) * totalBlocks) + (sizeof(Vertex) * vertices.size()) + (sizeof(GLuint) * indices.size())) / 1024.0));
}

void Chunk::Delete() {
    if (renderComponentsSetup) {
        vertexArrayObject.Delete();
        vertexBufferObject.Delete();
        indexBufferObject.Delete();
    }

    delete[] blocks;
    blocks = nullptr;

    vertices.clear();
    indices.clear();

    debugVisualizationVertices.clear();
    debugVisualizationIndices.clear();
}

Chunk::~Chunk() {
    Delete();
    totalChunks--;
}