#include <chrono>
#include <unordered_map>

#include <Chunk.h>
#include <VertexArrayObject.h>

Chunk::Chunk(int xPos, int yPos, int zPos) : xPos(xPos), yPos(yPos), zPos(zPos)
{
    //Block* blocks = new Block[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    //Block *blocks = malloc(CHUNK_SIZE * sizeof(*blocks));
    Block* blocks = new Block[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);  // Attribute 0 is for positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);  // Attribute 1 is for texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    // Unbind VAO to prevent accidental changes
    glBindVertexArray(0);

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
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                if (x + 1 <= CHUNK_SIZE - 1 && blocks[x + 1][y][z].IsSolid(x + 1, y, z, CHUNK_SIZE) == false && blocks[x][y][z].IsSolid(x, y, z, CHUNK_SIZE) == true) {
                    shouldAddFace[Right].shouldAdd = true;
                }
                else {
                    shouldAddFace[Right].shouldAdd = false;
                }
                if (x - 1 >= 0 && blocks[x - 1][y][z].IsSolid(x - 1, y, z, CHUNK_SIZE) == false && blocks[x][y][z].IsSolid(x, y, z, CHUNK_SIZE) == true) {
                    shouldAddFace[Left].shouldAdd = true;
                }
                else {
                    shouldAddFace[Left].shouldAdd = false;
                }
                if (y + 1 <= CHUNK_SIZE - 1 && blocks[x][y + 1][z].IsSolid(x, y + 1, z, CHUNK_SIZE) == false && blocks[x][y][z].IsSolid(x, y, z, CHUNK_SIZE) == true) {
                    shouldAddFace[Top].shouldAdd = true;
                }
                else {
                    shouldAddFace[Top].shouldAdd = false;
                }
                if (y - 1 >= 0 && blocks[x][y - 1][z].IsSolid(x, y - 1, z, CHUNK_SIZE) == false && blocks[x][y][z].IsSolid(x, y, z, CHUNK_SIZE) == true) {
                    shouldAddFace[Bottom].shouldAdd = true;
                }
                else {
                    shouldAddFace[Bottom].shouldAdd = false;
                }
                if (z + 1 <= CHUNK_SIZE - 1 && blocks[x][y][z + 1].IsSolid(x, y, z + 1, CHUNK_SIZE) == false && blocks[x][y][z].IsSolid(x, y, z, CHUNK_SIZE) == true) {
                    shouldAddFace[Back].shouldAdd = true;
                }
                else {
                    shouldAddFace[Back].shouldAdd = false;
                }
                if (z - 1 >= 0 && blocks[x][y][z - 1].IsSolid(x, y, z - 1, CHUNK_SIZE) == false && blocks[x][y][z].IsSolid(x, y, z, CHUNK_SIZE) == true) {
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
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);  // Attribute 0 is for positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);  // Attribute 1 is for texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);  // Unbind VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Chunk::~Chunk() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}