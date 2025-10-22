#include "ChunkHandler.h"
#include "World.h"
#include <chrono>
#include <klogger.hpp>


unsigned int Chunk::totalChunks = 0;


Chunk::Chunk(int chunkX, int chunkY, int chunkZ, ChunkHandler& chunkHandler) : chunkX(chunkX), chunkY(chunkY), chunkZ(chunkZ), chunkHandler(chunkHandler) {
    totalChunks++;
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

    heightmap.bitsPerColumn = static_cast<int>(std::ceil(std::log2(chunkSize)));
    heightmap.columnCount = chunkSize * chunkSize;
    heightmap.heightmap.resize(((heightmap.columnCount * heightmap.bitsPerColumn) + 63) / 64, 0);
    heightmap.heightmapMask.resize((heightmap.columnCount + 63) / 4, 0);

    isAllocated = true;
    generationStatus = 1;
}

bool Chunk::GenerateBlocks(World& world, Chunk* callerChunk, bool updateCallerChunk, bool debug) {
    if (isGenerated) {
        //std::cerr << "[Chunk Terrain Generation / Warn] Trying to generate blocks after they had already been generated, aborting {" << chunkX << ", " << chunkY << ", " << chunkZ << "}" << std::endl;
        return false;
    }
    if (!isAllocated) {
        CRITICAL("Trying to generate blocks for unallocated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks) {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}");
        psnip_trap();
        return false;
    }

    FastNoiseLite& noise = world.GetNoise();

    std::unordered_map<AssetStringID, unsigned short> blockIDCache;
    std::unordered_map<unsigned int, unsigned short> blockVariantCountCache;

    auto GetCachedID = [&](const AssetStringID& assetStringID) -> unsigned short {
        auto iterator = blockIDCache.find(assetStringID);
        if (iterator != blockIDCache.end()) {
            return iterator->second;
        }

        unsigned short numericalID = static_cast<unsigned short>(BlockManager::GetInstance().GetNumericalID(assetStringID));
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
                int reach = density * 16 + 30;
                if (!(height < reach)) {
                    block.blockID = 0;
		            block.variant = 0;
                    continue;
                }

                if (height + 4 < reach) {
                    block.blockID = GetCachedID(AssetStringID{"kiwicubed", "stone"});
		            block.variant = rand() % GetCachedVariantCount(block.blockID);
                    block.variant = 0;
                } else if (height + 1 < reach) {
                    block.blockID = GetCachedID(AssetStringID{"kiwicubed", "dirt"});
		            block.variant = 0;
                } else {
                    block.blockID = GetCachedID(AssetStringID{"kiwicubed", "grass"});
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
    generationStatus = 2;
    IsEmpty();
    IsFull();
    GenerateHeightmap();
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

    std::unordered_map<unsigned char, BlockType> textureAtlasDataCache;

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
                        const TextureAtlasData& atlasData = blockType.metaTextures[facesToAdd[iterator]].atlasData[variant];
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
    generationStatus = 3;
    shouldRender = true;

    std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
    int time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (Globals::GetInstance().debugMode) {
        INFO("Chunk meshing took " + std::to_string(time) + "ms");
    }

    return true;
}

bool Chunk::GenerateHeightmap() {
    OVERRIDE_LOG_NAME("Chunk Heightmap Generation");
    if (!isGenerated) {
        WARN("Tried to generate heightmap for ungenerated chunk {" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " + std::to_string(chunkZ) + "}");
    }

    int fullColumns = 0;

    for (int blockX = 0; blockX < chunkSize; blockX++) {
        for (int blockZ = 0; blockZ < chunkSize; blockZ++) {
            bool foundLevel = false;
            for (int blockY = chunkSize - 1; blockY >= 0 && foundLevel == false; blockY--) {
                if (!GetBlock(blockX, blockY, blockZ).IsAir()) {
                    int bitStart = (blockX + (blockZ * chunkSize)) * heightmap.bitsPerColumn;
                    int longIndex = bitStart / 64;
                    int bitOffset = bitStart % 64;

                    int maskLongIndex = (blockX + (blockZ * chunkSize)) / 64;
                    int maskBitIndex = (blockX + (blockZ * chunkSize)) % 64;

                    uint64_t mask = ((1ULL << heightmap.bitsPerColumn) - 1) << bitOffset;
                    heightmap.heightmap[longIndex] &= ~mask;
                    heightmap.heightmap[longIndex] |= (uint64_t(blockY) << bitOffset);

                    if (bitOffset + heightmap.bitsPerColumn > 64) {
                        int bitsInNext = bitOffset + heightmap.bitsPerColumn - 64;
                        heightmap.heightmap[longIndex + 1] &= ~((1ULL << bitsInNext) - 1);
                        heightmap.heightmap[longIndex + 1] |= uint64_t(blockY) >> (heightmap.bitsPerColumn - bitsInNext);
                    }

                    bool fullColumn = (blockY == chunkSize - 1);
                    if (fullColumn) {
                        heightmap.heightmapMask[maskLongIndex] |= (1ULL << maskBitIndex);
                        fullColumns++;
                    } else {
                        heightmap.heightmapMask[maskLongIndex] &= ~(1ULL << maskBitIndex);
                    }

                    foundLevel = true;
                }
            }
        }
    }

    return fullColumns == chunkSize * chunkSize;
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

    int index = position.x + (position.y * chunkSize);

    int maskLongIndex = index / 64;
    int maskBitIndex = index % 64;

    int full = (heightmap.heightmapMask[maskLongIndex] >> maskBitIndex) & 1ULL;

    if (full) {
        return -1;
    }

    int bitStart = index * heightmap.bitsPerColumn;
    int longIndex = bitStart / 64;
    int bitOffset = bitStart % 64;

    uint64_t value = (heightmap.heightmap[longIndex] >> bitOffset);
    if (bitOffset + heightmap.bitsPerColumn > 64) {
        int bitsInNext = bitOffset + heightmap.bitsPerColumn - 64;
        value |= heightmap.heightmap[longIndex + 1] << (heightmap.bitsPerColumn - bitsInNext);
    }

    return static_cast<int>(value & ((1ULL << heightmap.bitsPerColumn) - 1));

    return -1;
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
    ImGui::Text("Chunk position: {%d, %d, %d}, GS: %d, Blocks: %d, Vertices: %d, Indices: %d, VBO: %d, VAO: %d, IBO %d, Memory usage: %d KB, ID: %d", chunkX, chunkY, chunkZ, generationStatus, totalBlocks, static_cast<int>(vertices.size()), static_cast<int>(indices.size()), vertexBufferObject.vertexBufferObjectID, vertexArrayObject.vertexArrayObjectID, indexBufferObject.indexBufferObjectID, static_cast<int>(static_cast<float>((sizeof(Block) * totalBlocks) + (sizeof(Vertex) * vertices.size()) + (sizeof(GLuint) * indices.size())) / 1024.0), id);
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
}