#pragma once

#include <GLError.h>
#include <GLAD/glad.h>

#include <iostream>
#include <vector>

#include <chrono>
#include <unordered_map>

#include <Block.h>
#include <IndexBufferObject.h>
#include <Shader.h>
#include <VertexArrayObject.h>
#include <VertexBufferObject.h>




class World;


class Chunk {
public:
    const int chunkSize = 32;
    Block*** blocks;
    int chunkX;
    int chunkY;
    int chunkZ;
    bool isAllocated;
    bool isEmpty;
    float totalMemoryUsage;

    Chunk() : chunkX(0), chunkY(0), chunkZ(0), isAllocated(false), isEmpty(false), totalBlocks(0), totalMemoryUsage(0.0f), fPtrWorld(nullptr), startIndex(0), endIndex(0), chunkIndex(0), blocks(blocks) {}
    Chunk(int chunkX, int chunkY, int chunkZ);
    ~Chunk();

    void AllocateChunk();
    void GenerateBlocks();
    void GenerateMesh(World& World);
    void Render(Shader shaderProgram);

    void SetPosition(int newChunkX, int newChunkY, int newChunkZ);

    int GetTotalBlocks();

    std::vector<GLfloat> GetVertices();
    std::vector<GLuint> GetIndices();

    void SetStartIndex(int newStartIndex);
    int GetStartIndex();

    void SetEndIndex(int newEndIndex);
    int GetEndIndex();

    void SetChunkIndex(int newChunkIndex);
    int GetChunkIndex();


private:
    World* fPtrWorld;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    
    int totalBlocks;

    int startIndex;
    int endIndex;

    int chunkIndex;

    //VertexArrayObject vertexArrayObject;
    //VertexBufferObject vertexBufferObject;
    //IndexBufferObject indexBufferObject;

    bool IsArrayEmpty(Block*** blocks);
};