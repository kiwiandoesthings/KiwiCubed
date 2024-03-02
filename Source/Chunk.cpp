#include <iostream>

#include <chrono>
#include <unordered_map>

#include <Chunk.h>


struct ShouldAdd {
    bool shouldAdd;
};

std::unordered_map<FaceDirection, ShouldAdd> shouldAddFace;


Chunk::Chunk(int chunkX, int chunkY, int chunkZ)
{
    Chunk::chunkX = chunkX;
    Chunk::chunkY = chunkY;
    Chunk::chunkZ = chunkZ;
    blocks = new Block * *[chunkSize];
    for (int i = 0; i < chunkSize; ++i) {
        blocks[i] = new Block * [chunkSize];
        for (int j = 0; j < chunkSize; ++j) {
            blocks[i][j] = new Block[chunkSize];
        }
    }
}

void Chunk::AllocateChunk() {
    blocks = new Block * *[chunkSize];
    for (int i = 0; i < chunkSize; ++i) {
        blocks[i] = new Block * [chunkSize];
        for (int j = 0; j < chunkSize; ++j) {
            blocks[i][j] = new Block[chunkSize];
        }
    }

    isAllocated = true;
}

void Chunk::GenerateBlocks() {
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < chunkSize; ++x) {
        for (int y = 0; y < chunkSize; ++y) {
            for (int z = 0; z < chunkSize; ++z) {
                blocks[x][y][z].GenerateBlock(x, y, z, chunkX, chunkY, chunkZ, chunkSize);
                if (x + 1 <= chunkSize - 1) {
                    blocks[x + 1][y][z].GenerateBlock(x + 1, y, z, chunkX, chunkY, chunkZ, chunkSize);
                }
                if (x - 1 >= 0) {
                    blocks[x - 1][y][z].GenerateBlock(x - 1, y, z, chunkX, chunkY, chunkZ, chunkSize);
                }
                if (y + 1 <= chunkSize - 1) {
                    blocks[x][y + 1][z].GenerateBlock(x, y + 1, z, chunkX, chunkY, chunkZ, chunkSize);
                }
                if (y - 1 >= 0) {
                    blocks[x][y - 1][z].GenerateBlock(x, y - 1, z, chunkX, chunkY, chunkZ, chunkSize);
                }
                if (z + 1 <= chunkSize - 1) {
                    blocks[x][y][z + 1].GenerateBlock(x, y, z + 1, chunkX, chunkY, chunkZ, chunkSize);
                }
                if (z - 1 >= 0) {
                    blocks[x][y][z - 1].GenerateBlock(x, y, z - 1, chunkX, chunkY, chunkZ, chunkSize);
                }
            }
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "Chunk Generation / Info: World generation took " << duration << " microseconds" << std::endl;
}

void Chunk::GenerateMesh(World& world) {
    if (isAllocated == false) {
        std::cout << "Chunk Generation / Error: Trying to generate mesh for unallocated chunk" << std::endl;
        return;
    }
    // Clear existing data
    vertices.clear();
    indices.clear();

    vertices.reserve(24 * chunkSize * chunkSize * chunkSize);
    indices.reserve(36 * chunkSize * chunkSize * chunkSize);

    //auto start_time = std::chrono::high_resolution_clock::now();

    GenerateBlocks();

    auto start_time = std::chrono::high_resolution_clock::now();

    // Generate mesh for all the blocks in a chunk by querying blocks around it and marking faces to be generated
    for (int x = 0; x < chunkSize; ++x) {
        for (int y = 0; y < chunkSize; ++y) {
            for (int z = 0; z < chunkSize; ++z) {
                int currentBlockSolid = blocks[x][y][z].GetSolid();
                
                if (currentBlockSolid == 1) {
                    GenerateBlockMesh(x, y, z);
                }
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "Chunk Generation / Info: Chunk mesh generation took " << duration << " microseconds" << std::endl;
}

void Chunk::GenerateBlockMesh(int x, int y, int z) {
    if (x + 1 <= chunkSize - 1 && blocks[x + 1][y][z].GetSolid() == 0) {
        shouldAddFace[RIGHT].shouldAdd = true;
    }
    else {
        shouldAddFace[RIGHT].shouldAdd = false;
    }
    if (x - 1 >= 0 && blocks[x - 1][y][z].GetSolid() == 0) {
        shouldAddFace[LEFT].shouldAdd = true;
    }
    else {
        shouldAddFace[LEFT].shouldAdd = false;
    }
    if (y + 1 <= chunkSize - 1 && blocks[x][y + 1][z].GetSolid() == 0) {
        shouldAddFace[TOP].shouldAdd = true;
    }
    else {
        shouldAddFace[TOP].shouldAdd = false;
    }
    if (y - 1 >= 0 && blocks[x][y - 1][z].GetSolid() == 0) {
        shouldAddFace[BOTTOM].shouldAdd = true;
    }
    else {
        shouldAddFace[BOTTOM].shouldAdd = false;
    }
    if (z + 1 <= chunkSize - 1 && blocks[x][y][z + 1].GetSolid() == 0) {
        shouldAddFace[BACK].shouldAdd = true;
    }
    else {
        shouldAddFace[BACK].shouldAdd = false;
    }
    if (z - 1 >= 0 && blocks[x][y][z - 1].GetSolid() == 0) {
        shouldAddFace[FRONT].shouldAdd = true;
    }
    else {
        shouldAddFace[FRONT].shouldAdd = false;
    }

    if (shouldAddFace[FRONT].shouldAdd == true) {
        blocks[x][y][z].GenerateMesh(x, y, z, vertices, indices, FRONT);
    }
    if (shouldAddFace[BACK].shouldAdd == true) {
        blocks[x][y][z].GenerateMesh(x, y, z, Chunk::vertices, indices, BACK);
    }
    if (shouldAddFace[LEFT].shouldAdd == true) {
        blocks[x][y][z].GenerateMesh(x, y, z, Chunk::vertices, indices, LEFT);
    }
    if (shouldAddFace[RIGHT].shouldAdd == true) {
        blocks[x][y][z].GenerateMesh(x, y, z, Chunk::vertices, indices, RIGHT);
    }
    if (shouldAddFace[TOP].shouldAdd == true) {
        blocks[x][y][z].GenerateMesh(x, y, z, Chunk::vertices, indices, TOP);
    }
    if (shouldAddFace[BOTTOM].shouldAdd == true) {
        blocks[x][y][z].GenerateMesh(x, y, z, Chunk::vertices, indices, BOTTOM);
    }
}

// Ignore this old method
//void Chunk::Render(Shader shaderProgram) {
//    //glUniform3f(glGetUniformLocation(shaderProgram.shaderProgramID, "chunkPosition"), (GLfloat)chunkX, (GLfloat)chunkY, (GLfloat)chunkZ);
//    vertexArrayObject.Bind();
//    vertexBufferObject.Bind();
//    vertexBufferObject.Setup(vertices.size() * sizeof(GLfloat), vertices.data());
//    indexBufferObject.Bind();
//    indexBufferObject.Setup(indices.size() * sizeof(GLuint), indices.data());
//    vertexArrayObject.LinkAttribute(vertexBufferObject, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
//    vertexArrayObject.LinkAttribute(vertexBufferObject, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
//    //glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
//}

std::vector<GLfloat> Chunk::GetVertices() const {
    return Chunk::vertices;
}

std::vector<GLuint> Chunk::GetIndices() const {
    return Chunk::indices;
}

int Chunk::GetStartIndex() const {
    return startIndex;
}

int Chunk::GetEndIndex() const {
    return endIndex;
}

void Chunk::SetStartIndex(int newStartIndex) {
    startIndex = newStartIndex;
}

void Chunk::SetEndIndex(int newEndIndex) {
    endIndex = newEndIndex;
}

Chunk::~Chunk() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}