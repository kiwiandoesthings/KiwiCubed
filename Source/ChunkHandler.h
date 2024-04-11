#pragma once

#include <GLError.h>
#include <glad/glad.h>

#include <iostream>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <Block.h>
#include <IndexBufferObject.h>
#include <Shader.h>
#include <VertexArrayObject.h>
#include <VertexBufferObject.h>


const int chunkSize = 32;

class Chunk;

// I don't even kind of know what is happening here
// Seriously what the hell
struct TripleHash {
    template <class T1, class T2, class T3>
    std::size_t operator() (const std::tuple<T1, T2, T3>& tuple) const {
        return std::hash<T1>()(std::get<0>(tuple)) ^ std::hash<T2>()(std::get<1>(tuple)) ^ std::hash<T3>()(std::get<2>(tuple));
    }
};


class ChunkHandler {
    private:
    
    public:
        ChunkHandler();
        void Delete();

        Chunk* GetChunk(int chunkX, int chunkY, int chunkZ);

        std::unordered_map<std::tuple<int, int, int>, Chunk, TripleHash> chunks;
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
        bool isEmpty;
        float totalMemoryUsage;
    
        Chunk(int chunkX, int chunkY, int chunkZ);
        ~Chunk();
    
        void AllocateChunk();
        void GenerateBlocks();
        void GenerateMesh(ChunkHandler& sparseVoxelOctree);
        void Render(Shader shaderProgram);
    
        void SetPosition(int newChunkX, int newChunkY, int newChunkZ);
    
        int GetTotalBlocks();
    
        std::vector<GLfloat> GetVertices();
        std::vector<GLuint> GetIndices();
        
        // Deprecated stuffs
        //void SetStartIndex(int newStartIndex);
        //int GetStartIndex();
        //
        //void SetEndIndex(int newEndIndex);
        //int GetEndIndex();
        //
        //void SetChunkIndex(int newChunkIndex);
        //int GetChunkIndex();
    
    
    private:
        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;
    
        int totalBlocks;
    
        //int startIndex;
        //int endIndex;
        //
        //int chunkIndex;
    
        VertexArrayObject vertexArrayObject;
        VertexBufferObject vertexBufferObject;
        IndexBufferObject indexBufferObject;
    
        bool IsArrayEmpty(Block*** blocks);
};