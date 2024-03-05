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


struct ShouldAdd {
    bool shouldAdd;
};


class World;


class Chunk {
public:
    static const int chunkSize = 32;
    Block*** blocks;
    int chunkX;
    int chunkY;
    int chunkZ;
    bool isAllocated;

    Chunk() : chunkX(0), chunkY(0), chunkZ(0), isAllocated(false), fPtrWorld(nullptr), startIndex(0), endIndex(0), blocks(blocks) {}
    Chunk(int chunkX, int chunkY, int chunkZ);
    ~Chunk();

    void AllocateChunk();
    void GenerateBlocks();
    void GenerateMesh(World& World);
    void GenerateBlockMesh(int x, int y, int z);
    void Render(/*Shader shaderProgram*/);

    std::vector<GLfloat> GetVertices() const;
    std::vector<GLuint> GetIndices() const;

    int GetStartIndex() const;
    int GetEndIndex() const;
    void SetStartIndex(int newStartIndex);
    void SetEndIndex(int newEndIndex);

private:
    World* fPtrWorld;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;  

    int startIndex;
    int endIndex;

	VertexArrayObject vertexArrayObject;
	VertexBufferObject vertexBufferObject;
	IndexBufferObject indexBufferObject;
};