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
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "ImGui.h"

#include "Block.h"
#include "IndexBufferObject.h"
#include "Renderer.h"
#include "Shader.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"


const int chunkSize = 32;


class Chunk;
class ChunkHandler;
class World;

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
        Block*** blocks = nullptr;
        int chunkX = 0;
        int chunkY = 0;
        int chunkZ = 0;
        bool isAllocated = false;
        bool isGenerated = false;
        bool isMeshed = false;
        unsigned int generationStatus = 0;
        bool isEmpty = true;
        bool isFull;
        bool shouldGenerate = true;
        unsigned int totalMemoryUsage;
        bool id = 0;

        Chunk() {}
        Chunk(int chunkX, int chunkY, int chunkZ) : chunkX(chunkX), chunkY(chunkY), chunkZ(chunkZ), isEmpty(false) {}



        void SetupRenderComponents();
        void AllocateChunk();
        void GenerateBlocks(World& world, Chunk& callerChunk, bool updateCallerChunk, bool debug);
        void GenerateMesh(ChunkHandler& chunkHandler, const bool remesh);
        void Render();

        void SetPosition(int newChunkX, int newChunkY, int newChunkZ);
    
        int GetTotalBlocks() const;
        void SetTotalBlocks(unsigned short newTotalBlocks);
    
        std::vector<GLfloat>& GetVertices();
        std::vector<GLuint>& GetIndices();

        std::vector<GLfloat>& GetDebugVisualizationVertices();
        std::vector<GLuint>& GetDebugVisualizationIndices();

        unsigned int GetMemoryUsage();

        bool IsEmpty();
        bool IsFull();

        void DisplayImGui() const;

        void Delete();
    
    private:
        const int debugVertexScale = 1;

        Renderer renderer;

        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;

        std::vector<GLfloat> debugVisualizationVertices;
        std::vector<GLuint> debugVisualizationIndices;
    
        unsigned short totalBlocks;
    
        VertexArrayObject vertexArrayObject;
        VertexBufferObject vertexBufferObject;
        IndexBufferObject indexBufferObject;
};


class ChunkHandler {
public:
    std::unordered_map<std::tuple<int, int, int>, Chunk, TripleHash> chunks;

    ChunkHandler(World& world) : world(world) {};
    void Delete();

    void GenerateWorld();

    Chunk& GetChunk(int chunkX, int chunkY, int chunkZ);
    Chunk& AddChunk(int chunkX, int chunkY, int chunkZ);
    void GenerateChunk(int chunkX, int chunkY, int chunkZ, bool debug);
    void MeshChunk(int chunkX, int chunkY, int chunkZ);
    void SmartGenerateAndMeshChunk(int chunkX, int chunkY, int chunkZ);
    void ForceGenerateAndMeshChunk(int chunkX, int chunkY, int chunkZ);
    void RemeshChunk(int chunkX, int chunkY, int chunkZ, bool updateNeighbors);

    void AddBlock(int chunkX, int chunkY, int chunkZ, int blockX, int blockY, int blockZ, unsigned short newBlockID);
    void RemoveBlock(int chunkX, int chunkY, int chunkZ, int blockX, int blockY, int blockZ);

private:
    World& world;

    Chunk defaultChunk = Chunk(0, 0, 0);
};