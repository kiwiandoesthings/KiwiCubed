//#include <ChunkOld.h>
//
//
//Chunk::Chunk(int chunkX, int chunkY, int chunkZ) : chunkX(0), chunkY(0), chunkZ(0), isAllocated(false), isEmpty(false), totalBlocks(0), fPtrWorld(nullptr), startIndex(0), endIndex(0), chunkIndex(0), blocks(blocks) {
//    Chunk::chunkX = chunkX;
//    Chunk::chunkY = chunkY;
//    Chunk::chunkZ = chunkZ;
//}
//
//void Chunk::AllocateChunk() {
//    if (!isAllocated) {
//        // Allocate memory for all the blocks in the chunk
//        blocks = new Block * *[chunkSize];
//        for (int i = 0; i < chunkSize; ++i) {
//            blocks[i] = new Block * [chunkSize];
//            for (int j = 0; j < chunkSize; ++j) {
//                blocks[i][j] = new Block[chunkSize];
//                totalBlocks += chunkSize;
//            }
//        }
//        isAllocated = true;
//    }
//}
//
//void Chunk::GenerateBlocks() {
//    // Generates all the blocks in the chunk
//    //auto start_time = std::chrono::high_resolution_clock::now();
//    for (int x = 0; x < chunkSize; ++x) {
//        for (int y = 0; y < chunkSize; ++y) {
//            for (int z = 0; z < chunkSize; ++z) {
//                blocks[x][y][z].GenerateBlock(x, y, z, chunkX, chunkY, chunkZ, chunkSize);
//            }
//        }
//    }
//    totalMemoryUsage = (float)sizeof(Chunk) + (sizeof(Block) * totalBlocks) + sizeof(vertices) + sizeof(indices);
//    //auto end_time = std::chrono::high_resolution_clock::now();
//    //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
//    //std::cout << "Chunk World Generation / Info: Chunk world generation took " << duration << " ms" << std::endl;
//}
//
//void Chunk::GenerateMesh(World& world) {
//    if (!isAllocated) {
//        std::cout << "Chunk Mesh Generation / Error: Trying to generate mesh for unallocated chunk, aborting. (This should never happen, report a bug if you encounter this, thanks)" << std::endl;
//        return;
//    }
//
//    // Clear existing data
//    vertices.clear();
//    indices.clear();
//
//    GenerateBlocks();
//
//    if (IsArrayEmpty(blocks)) {
//        isEmpty = true;
//        //std::cout << "Chunk Mesh Generation / Info: Chunk is empty, skipping mesh generation" << std::endl;
//        return;
//    }
//    
//    isEmpty = false;
//
//    Chunk positiveXChunk = world.GetChunk(chunkX + 1, chunkY, chunkZ);
//    Chunk negativeXChunk = world.GetChunk(chunkX - 1, chunkY, chunkZ);
//    Chunk positiveYChunk = world.GetChunk(chunkX, chunkY + 1, chunkZ);
//    Chunk negativeYChunk = world.GetChunk(chunkX, chunkY - 1, chunkZ);
//    Chunk positiveZChunk = world.GetChunk(chunkX, chunkY, chunkZ + 1);
//    Chunk negativeZChunk = world.GetChunk(chunkX, chunkY, chunkZ - 1);
//
//    //auto start_time = std::chrono::high_resolution_clock::now();
//
//    // Generate mesh for all the blocks in a chunk by querying blocks around it and marking faces to be generated
//    for (int x = 0; x < chunkSize; ++x) {
//        for (int y = 0; y < chunkSize; ++y) {
//            for (int z = 0; z < chunkSize; ++z) {
//    
//                if (blocks[x][y][z].GetType() == 1) {
//                    Block& block = blocks[x][y][z];
//                    // Check each face for blocks around it and generate mesh for faces that should be added
//                    //if (x + 1 > chunkSize - 1) {
//                    //    if (!positiveXChunk.isAllocated) {
//                    //        positiveXChunk.AllocateChunk();
//                    //    }
//                    //    positiveXChunk.AllocateChunk();
//                    //    std::cout << chunkX << " " << chunkY << " " << chunkZ << std::endl;
//                    //    if (positiveXChunk.blocks[0][y][z].GetType() == 0) {
//                    //        block.AddFace(vertices, indices, RIGHT, chunkX, chunkY, chunkZ, chunkSize);
//                    //    }
//                    //}
//                    //else if (blocks[x + 1][y][z].GetType() == 0) {
//                    //    block.AddFace(vertices, indices, RIGHT, chunkX, chunkY, chunkZ, chunkSize);
//                    //}
//                        
//
//
//
//                    if (x - 1 >= 0 && blocks[x - 1][y][z].GetType() == 0) {
//                        block.AddFace(vertices, indices, LEFT,   chunkX, chunkY, chunkZ, chunkSize);
//                    }
//                    if (y + 1 <= chunkSize - 1 && blocks[x][y + 1][z].GetType() == 0) {
//                        block.AddFace(vertices, indices, TOP,    chunkX, chunkY, chunkZ, chunkSize);
//                    }
//                    if (y - 1 >= 0 && blocks[x][y - 1][z].GetType() == 0) {
//                        block.AddFace(vertices, indices, BOTTOM, chunkX, chunkY, chunkZ, chunkSize);
//                    }
//                    if (z + 1 <= chunkSize - 1 && blocks[x][y][z + 1].GetType() == 0) {
//                        block.AddFace(vertices, indices, BACK,   chunkX, chunkY, chunkZ, chunkSize);
//                    }
//                    if (z - 1 >= 0 && blocks[x][y][z - 1].GetType() == 0) {
//                        block.AddFace(vertices, indices, FRONT,  chunkX, chunkY, chunkZ, chunkSize);
//                    }
//                }
//            }
//        }
//    }
//
//    //auto end_time = std::chrono::high_resolution_clock::now();
//    //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
//    //std::cout << "Chunk Generation / Info: Chunk mesh generation took " << duration << " ms" << std::endl;
//}
//
//// This method is already deprecated, and should only be used for debugging purposes
//void Chunk::Render(Shader shaderProgram) {
//    glUniform3f(glGetUniformLocation(shaderProgram.shaderProgramID, "chunkPosition"), (GLfloat)chunkX, (GLfloat)chunkY, (GLfloat)chunkZ);
//    vertexArrayObject.Bind();
//    vertexBufferObject.Bind();
//    vertexBufferObject.Setup(vertices.size() * sizeof(GLfloat), vertices.data());
//    indexBufferObject.Bind();
//    indexBufferObject.Setup(indices.size() * sizeof(GLuint), indices.data());
//    vertexArrayObject.LinkAttribute(vertexBufferObject, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
//    vertexArrayObject.LinkAttribute(vertexBufferObject, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
//    GLCall(glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0));
//}
//
//void Chunk::SetPosition(int newChunkX, int newChunkY, int newChunkZ) {
//    chunkX = newChunkX;
//    chunkY = newChunkY;
//    chunkZ = newChunkZ;
//}
//
//int Chunk::GetTotalBlocks() {
//    return totalBlocks;
//}
//
//std::vector<GLfloat> Chunk::GetVertices() {
//    return Chunk::vertices;
//}
//
//std::vector<GLuint> Chunk::GetIndices() {
//    return Chunk::indices;
//}
//
//void Chunk::SetStartIndex(int newStartIndex) {
//    startIndex = newStartIndex;
//}
//
//int Chunk::GetStartIndex() {
//    return startIndex;
//}
//
//void Chunk::SetEndIndex(int newEndIndex) {
//    endIndex = newEndIndex;
//}
//
//int Chunk::GetEndIndex() {
//    return endIndex;
//}
//
//void Chunk::SetChunkIndex(int newChunkIndex) {
//	chunkIndex = newChunkIndex;
//}
//
//int Chunk::GetChunkIndex() {
//    return chunkIndex;
//}
//
//bool Chunk::IsArrayEmpty(Block*** blocks) {
//    for (int x = 0; x < chunkSize; ++x) {
//        for (int y = 0; y < chunkSize; ++y) {
//            for (int z = 0; z < chunkSize; ++z) {
//                if (blocks[x][y][z].GetType() == 1) {
//                    return false;
//                }
//            }
//        }
//    }
//
//    return true;
//}
//
//Chunk::~Chunk() {
//    //xvertexArrayObject.Delete();
//    //vertexBufferObject.Delete();
//    //indexBufferObject.Delete();
//}