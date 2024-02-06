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
    static const int CHUNK_SIZE = 32;
    Block*** blocks;
    int xPos;
    int yPos;
    int zPos;
    Chunk(int x, int y, int z);
    ~Chunk();

    void GenerateMesh(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices);
    void Render();

private:
    std::vector<GLfloat> vertices;  // Vertex data
    std::vector<GLuint> indices;    // Index data

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};