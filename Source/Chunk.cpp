#include <chrono>
#include <unordered_map>

#include <Chunk.h>
#include <VertexArrayObject.h>


Chunk::Chunk(int x, int y, int z)
{
    chunkX = x;
    chunkY = y;
    chunkZ = z;
    blocks = new Block * *[chunkSize];
    for (int i = 0; i < chunkSize; ++i) {
        blocks[i] = new Block * [chunkSize];
        for (int j = 0; j < chunkSize; ++j) {
            blocks[i][j] = new Block[chunkSize];
        }
    }

    GenerateMesh(vertices, indices);
}

void Chunk::GenerateMesh(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices) {
    // Clear existing data
    vertices.clear();
    indices.clear();

    struct ShouldAdd {
        bool shouldAdd;
    };

    enum FaceDirection {
        Front,
        Back,
        Left,
        Right,
        Top,
        Bottom
    };

    std::unordered_map<FaceDirection, ShouldAdd> shouldAddFace;

    auto start_time = std::chrono::high_resolution_clock::now();
    // Generate mesh for all the blocks in a chunk by querying blocks around it and marking faces to be generated
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

                if (x + 1 <= chunkSize - 1 && blocks[x + 1][y][z].GetSolid() == 0 && blocks[x][y][z].GetSolid() == 1) {
                    shouldAddFace[Right].shouldAdd = true;
                }
                else {
                    shouldAddFace[Right].shouldAdd = false;
                }
                if (x - 1 >= 0 && blocks[x - 1][y][z].GetSolid() == 0 && blocks[x][y][z].GetSolid() == 1) {
                    shouldAddFace[Left].shouldAdd = true;
                }
                else {
                    shouldAddFace[Left].shouldAdd = false;
                }
                if (y + 1 <= chunkSize - 1 && blocks[x][y + 1][z].GetSolid() == 0 && blocks[x][y][z].GetSolid() == 1) {
                    shouldAddFace[Top].shouldAdd = true;
                }
                else {
                    shouldAddFace[Top].shouldAdd = false;
                }
                if (y - 1 >= 0 && blocks[x][y - 1][z].GetSolid() == 0 && blocks[x][y][z].GetSolid() == 1) {
                    shouldAddFace[Bottom].shouldAdd = true;
                }
                else {
                    shouldAddFace[Bottom].shouldAdd = false;
                }
                if (z + 1 <= chunkSize - 1 && blocks[x][y][z + 1].GetSolid() == 0 && blocks[x][y][z].GetSolid() == 1) {
                    shouldAddFace[Back].shouldAdd = true;
                }
                else {
                    shouldAddFace[Back].shouldAdd = false;
                }
                if (z - 1 >= 0 && blocks[x][y][z - 1].GetSolid() == 0 && blocks[x][y][z].GetSolid() == 1) {
                    shouldAddFace[Front].shouldAdd = true;
                }
                else {
                    shouldAddFace[Front].shouldAdd = false;
                }

                if (shouldAddFace[Front].shouldAdd == true) {
                    blocks[x][y][z].GenerateMesh(x, y, z, vertices, indices, FRONT);
                }
                if (shouldAddFace[Back].shouldAdd == true) {
                    blocks[x][y][z].GenerateMesh(x, y, z, vertices, indices, BACK);
                }
                if (shouldAddFace[Left].shouldAdd == true) {
                    blocks[x][y][z].GenerateMesh(x, y, z, vertices, indices, LEFT);
                }
                if (shouldAddFace[Right].shouldAdd == true) {
                    blocks[x][y][z].GenerateMesh(x, y, z, vertices, indices, RIGHT);
                }
                if (shouldAddFace[Top].shouldAdd == true) {
                    blocks[x][y][z].GenerateMesh(x, y, z, vertices, indices, TOP);
                }
                if (shouldAddFace[Bottom].shouldAdd == true) {
                    blocks[x][y][z].GenerateMesh(x, y, z, vertices, indices, BOTTOM);
                }
            }
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "Client World/ Info: Chunk mesh generation took " << duration / 1000 << " milliseconds" << std::endl;
}

void Chunk::Render() {
    vertexArrayObject.Bind();
    vertexBufferObject.Bind();
    vertexBufferObject.Setup(vertices.size() * sizeof(GLfloat), vertices.data());
    indexBufferObject.Bind();
    indexBufferObject.Setup(indices.size() * sizeof(GLuint), indices.data());
    vertexArrayObject.LinkAttribute(vertexBufferObject, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    vertexArrayObject.LinkAttribute(vertexBufferObject, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

Chunk::~Chunk() {
    vertexArrayObject.Delete();
    vertexBufferObject.Delete();
    indexBufferObject.Delete();
}