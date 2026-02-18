#pragma once

#include "GLError.h"
#include <glad/glad.h>
#include <klogger.hpp>

#include <algorithm>
#include <atomic>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "imgui.h"

#include "Block.h"
#include "IndexBufferObject.h"
#include "Renderer.h"
#include "Shader.h"
#include "AssetManager.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"


const unsigned char chunkSize = 32;


class Chunk;
class ChunkHandler;
class World;


struct ChunkHeightmap {
    unsigned char heightmap[chunkSize][chunkSize];
    bool heightmapMask[chunkSize][chunkSize];
};


// I don't even kind of know what is happening here
// Seriously what the hell
struct TripleHash {
    template <class T1, class T2, class T3>
    std::size_t operator() (const std::tuple<T1, T2, T3>& tuple) const {
        return std::hash<T1>()(std::get<0>(tuple)) ^ std::hash<T2>()(std::get<1>(tuple)) ^ std::hash<T3>()(std::get<2>(tuple));
    }
};


class Chunk {
    public:
        static unsigned int totalChunks;
        Block* blocks = new Block[chunkSize * chunkSize * chunkSize];
        ChunkHeightmap heightmap;
        int chunkX = -1;
        int chunkY = -1;
        int chunkZ = -1;
        bool isAllocated = false;
        bool isGenerated = false;
        bool isMeshed = false;
        bool isEmpty = true;
        bool isFull = false;
        bool renderComponentsSetup = false;
        bool shouldRender = false;
        bool shouldGenerate = true;
        bool shouldDelete = false;
        unsigned int totalMemoryUsage;
        ChunkHandler& chunkHandler;

        Chunk(int chunkX, int chunkY, int chunkZ, ChunkHandler& chunkHandler);
        ~Chunk();

        Chunk(const Chunk&) = delete;
        Chunk& operator=(const Chunk&) = delete;
        Chunk(Chunk&& other) noexcept;
        Chunk& operator=(Chunk&& other) noexcept;

        bool IsReal();
		unsigned char GetGenerationStatus();
        void SetupRenderComponents();
        void AllocateChunk();
        bool GenerateBlocks(World& world, Chunk* callerChunk, bool updateCallerChunk, bool debug);
        bool GenerateMesh(const bool remesh);
        void GenerateHeightmap();
        void Render();

        inline Block& GetBlock(const unsigned char blockX, const unsigned char blockY, const unsigned char blockZ) {
            return blocks[blockX + chunkSize * (blockY + chunkSize * blockZ)];
        }

        void SetPosition(int newChunkX, int newChunkY, int newChunkZ);

        int GetTotalBlocks() const;
        void SetTotalBlocks(unsigned short newTotalBlocks);

        bool GetMeshable(ChunkHandler& chunkHandler) const;

        int GetHeightmapLevelAt(glm::ivec2 position);
    
        std::vector<GLfloat>& GetVertices();
        std::vector<GLuint>& GetIndices();

        std::vector<GLfloat>& GetDebugVisualizationVertices();
        std::vector<GLuint>& GetDebugVisualizationIndices();

        unsigned int GetMemoryUsage();

        bool IsEmpty();
        bool IsFull();

        void DisplayImGui();

        void Delete();
    
    private:
        bool isRealChunk = false;
        int debugVertexScale = 1;

        Renderer renderer;

        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;

        std::vector<GLfloat> debugVisualizationVertices;
        std::vector<GLuint> debugVisualizationIndices;
    
        unsigned int totalBlocks = 0;
    
        VertexArrayObject vertexArrayObject;
        VertexBufferObject vertexBufferObject;
        IndexBufferObject indexBufferObject;
};


class ChunkHandler {
    public:
        std::unordered_map<std::tuple<int, int, int>, std::unique_ptr<Chunk>, TripleHash> chunks;
        std::mutex ChunkMutex;

        ChunkHandler(World& world);
        void Delete();

        void GenerateWorld();
        void CleanChunks();

        Chunk* GetChunk(int chunkX, int chunkY, int chunkZ, bool addIfNotFound);
        Chunk* GetChunkUnlocked(int chunkX, int chunkY, int chunkZ, bool addIfNotFound);
        Chunk* AddChunk(int chunkX, int chunkY, int chunkZ);
        Chunk* AddChunkUnlocked(int chunkX, int chunkY, int chunkZ);
        bool GetChunkExists(int chunkX, int chunkY, int chunkZ);
        void GenerateChunk(int chunkX, int chunkY, int chunkZ, Chunk* callerChunk, bool updateCallerChunk, bool debug);
        bool MeshChunk(int chunkX, int chunkY, int chunkZ);
        void SmartGenerateAndMeshChunk(int chunkX, int chunkY, int chunkZ);
        void ForceGenerateAndMeshChunk(int chunkX, int chunkY, int chunkZ);
        void RemeshChunk(int chunkX, int chunkY, int chunkZ, bool updateNeighbors);

        void AddBlock(int chunkX, int chunkY, int chunkZ, int blockX, int blockY, int blockZ, unsigned short newBlockID);
        void RemoveBlock(int chunkX, int chunkY, int chunkZ, int blockX, int blockY, int blockZ);

        static int MakeUniqueIDFromVec3(int x, int y, int z);
        static int MakeUniqueIDFromVec2(int x, int y);

    private:
        World& world;

        std::unique_ptr<Chunk> defaultChunk = nullptr;
};