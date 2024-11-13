#pragma once

#include "GLError.h"
#include <glad/glad.h>

#include <iostream>
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
class World;

// I don't even kind of know what is happening here
// Seriously what the hell
struct TripleHash {
    template <class T1, class T2, class T3>
    std::size_t operator() (const std::tuple<T1, T2, T3>& tuple) const {
        return std::hash<T1>()(std::get<0>(tuple)) ^ std::hash<T2>()(std::get<1>(tuple)) ^ std::hash<T3>()(std::get<2>(tuple));
    }
};


class ChunkHandler {
    public:
        std::unordered_map<std::tuple<int, int, int>, Chunk, TripleHash> chunks;

        ChunkHandler(World& world);
        void Delete();

        Chunk& GetChunk(int chunkX, int chunkY, int chunkZ);
        void AddChunk(int chunkX, int chunkY, int chunkZ);
        void GenerateAndMeshChunk(int chunkX, int chunkY, int chunkZ);

    private:
        World& world;
};

class Chunk {
    public:
        Block*** blocks;
        int chunkX;
        int chunkY;
        int chunkZ;
        bool isAllocated = false;
        bool isGenerated = false;
        bool isMeshed = false;
        unsigned char generationStatus = 0;
        bool isEmpty;
        bool shouldGenerate = true;
        unsigned int totalMemoryUsage;
    
        Chunk() : blocks(nullptr), chunkX(0), chunkY(0), chunkZ(0), isAllocated(false), isGenerated(false), isMeshed(false), isEmpty(false), totalMemoryUsage(0), totalBlocks(0)/*, vertexBufferObject(("chunk " + std::to_string(chunkX) + " " + std::to_string(chunkY) + " " + std::to_string(chunkZ)).c_str())*/ {}
        Chunk(int chunkX, int chunkY, int chunkZ);

        ~Chunk();
    
        void AllocateChunk();
        void GenerateBlocks(World world, Chunk& callerChunk, bool updateCallerChunk);
        void GenerateMesh(ChunkHandler& chunkHandler);
        void Render();
    
        void SetPosition(int newChunkX, int newChunkY, int newChunkZ);
    
        int GetTotalBlocks() const;
    
        std::vector<GLfloat> GetVertices() const;
        std::vector<GLuint> GetIndices() const;

        unsigned int GetMemoryUsage() const;

        bool IsEmpty();

        void DisplayImGui() const;

        //void Delete() const;
    
    private:
        Renderer renderer;

        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;
    
        unsigned short totalBlocks;
    
        VertexArrayObject vertexArrayObject;
        VertexBufferObject vertexBufferObject;
        IndexBufferObject indexBufferObject;
};