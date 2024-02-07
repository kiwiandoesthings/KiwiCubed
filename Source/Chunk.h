#pragma once

#include <glad/glad.h>

#include <iostream>
#include <vector>

#include <Block.h>
#include <IndexBufferObject.h>
#include <VertexArrayObject.h>
#include <VertexBufferObject.h>

class Chunk {
public:
    static const int chunkSize = 32;
    Block*** blocks;
    int chunkX;
    int chunkY;
    int chunkZ;


    Chunk(int x, int y, int z);
    ~Chunk();

    void GenerateMesh(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices);
    void Render();

private:
    std::vector<GLfloat> vertices;  // Vertex data
    std::vector<GLuint> indices;    // Index data

	VertexArrayObject vertexArrayObject;
	VertexBufferObject vertexBufferObject;
	IndexBufferObject indexBufferObject;
};