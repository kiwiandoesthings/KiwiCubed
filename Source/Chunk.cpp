#include <Chunk.h>


std::unordered_map<FaceDirection, ShouldAdd> shouldAddFace;


Chunk::Chunk(int chunkX, int chunkY, int chunkZ) : chunkX(0), chunkY(0), chunkZ(0), isAllocated(false), fPtrWorld(nullptr), startIndex(0), endIndex(0), blocks(blocks) {
    Chunk::chunkX = chunkX;
    Chunk::chunkY = chunkY;
    Chunk::chunkZ = chunkZ;
}

void Chunk::AllocateChunk() {
    // Allocate memory for all the blocks in the chunk
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
    // Generates all the blocks in the chunk
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < chunkSize; ++x) {
        for (int y = 0; y < chunkSize; ++y) {
            for (int z = 0; z < chunkSize; ++z) {
                blocks[x][y][z].GenerateBlock(x, y, z, chunkX, chunkY, chunkZ, chunkSize);
            }
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "Chunk Generation / Info: Chunk world generation took " << duration << " ms" << std::endl;
}

void Chunk::GenerateMesh(World& world) {
    if (isAllocated == false) {
        std::cout << "Chunk Generation / Error: Trying to generate mesh for unallocated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks)" << std::endl;
        return;
    }

    // Clear existing data
    vertices.clear();
    indices.clear();

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
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "Chunk Generation / Info: Chunk mesh generation took " << duration << " ms" << std::endl;
}

void Chunk::GenerateBlockMesh(int x, int y, int z) {
    // Clear all the faces that were added
    for (auto& entry : shouldAddFace) {
        entry.second.shouldAdd = false;
    }

    // Check each face for blocks around it and generate mesh for faces that should be added
    if (x + 1 <= chunkSize - 1 && blocks[x + 1][y][z].GetSolid() == 0) {
        shouldAddFace[RIGHT].shouldAdd = true;
        blocks[x][y][z].AddFace(x, y, z, vertices, indices, RIGHT);
    }
    if (x - 1 >= 0 && blocks[x - 1][y][z].GetSolid() == 0) {
        shouldAddFace[LEFT].shouldAdd = true;
        blocks[x][y][z].AddFace(x, y, z, vertices, indices, LEFT);
    }
    if (y + 1 <= chunkSize - 1 && blocks[x][y + 1][z].GetSolid() == 0) {
        shouldAddFace[TOP].shouldAdd = true;
        blocks[x][y][z].AddFace(x, y, z, vertices, indices, TOP);
    }
    if (y - 1 >= 0 && blocks[x][y - 1][z].GetSolid() == 0) {
        shouldAddFace[BOTTOM].shouldAdd = true;
        blocks[x][y][z].AddFace(x, y, z, vertices, indices, BOTTOM);
    }
    if (z + 1 <= chunkSize - 1 && blocks[x][y][z + 1].GetSolid() == 0) {
        shouldAddFace[BACK].shouldAdd = true;
        blocks[x][y][z].AddFace(x, y, z, vertices, indices, BACK);
    }
    if (z - 1 >= 0 && blocks[x][y][z - 1].GetSolid() == 0) {
        shouldAddFace[FRONT].shouldAdd = true;
        blocks[x][y][z].AddFace(x, y, z, vertices, indices, FRONT);
    }
}

// Ignore this old method
void Chunk::Render(/*Shader shaderProgram*/) {
    //glUniform3f(glGetUniformLocation(shaderProgram.shaderProgramID, "chunkPosition"), (GLfloat)chunkX, (GLfloat)chunkY, (GLfloat)chunkZ);
    vertexArrayObject.Bind();
    vertexBufferObject.Bind();
    vertexBufferObject.Setup(vertices.size() * sizeof(GLfloat), vertices.data());
    indexBufferObject.Bind();
    indexBufferObject.Setup(indices.size() * sizeof(GLuint), indices.data());
    vertexArrayObject.LinkAttribute(vertexBufferObject, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    vertexArrayObject.LinkAttribute(vertexBufferObject, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    GLCall(glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0));
}

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
    vertexArrayObject.Delete();
    vertexBufferObject.Delete();
    indexBufferObject.Delete();
}