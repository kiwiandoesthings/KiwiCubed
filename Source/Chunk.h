#pragma once

#include <glad/glad.h>

#include <vector>

#include <Block.h>
#include <IndexBufferObject.h>
#include <Shader.h>
#include <VertexArrayObject.h>
#include <VertexBufferObject.h>


class World;


class Chunk {
public:
    static const int chunkSize = 32;
    Block*** blocks;
    int chunkX;
    int chunkY;
    int chunkZ;
    bool isAllocated;

    Chunk() : chunkX(0), chunkY(0), chunkZ(0), blocks(0), isAllocated(false), fPtrWorld(fPtrWorld) {}
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
    std::vector<GLfloat> vertices;  // Vertex data
    std::vector<GLuint> indices;    // Index data

    int startIndex;
    int endIndex;

	VertexArrayObject vertexArrayObject;
	VertexBufferObject vertexBufferObject;
	IndexBufferObject indexBufferObject;
};