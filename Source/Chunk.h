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
    static const int CHUNK_SIZE = 48;
    Chunk(int xPos, int yPos, int zPos);
    ~Chunk();

    void GenerateMesh(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices);
    void Render();

    // Methods for manipulating and rendering the chunk
    // ...

private:
    Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

    int xPos;
    int yPos;
    int zPos;

    std::vector<GLfloat> vertices;  // Vertex data
    std::vector<GLuint> indices;    // Index data

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};