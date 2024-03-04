#include <World.h>


World::World() {
    chunks = new Chunk * *[worldSize];
    for (int i = 0; i < worldSize; ++i) {
        chunks[i] = new Chunk * [worldSize];
        for (int j = 0; j < worldSize; ++j) {
            chunks[i][j] = new Chunk[worldSize];
        }
    }
}

void World::Render() {
    //vertexArrayObject.Bind();
    //vertexBufferObject.Bind();
    //vertexBufferObject.Setup(vertices.size() * sizeof(GLfloat), vertices.data());
    //indexBufferObject.Bind();
    //indexBufferObject.Setup(indices.size() * sizeof(GLuint), indices.data());
    //vertexArrayObject.LinkAttribute(vertexBufferObject, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    //vertexArrayObject.LinkAttribute(vertexBufferObject, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    //glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

    chunks[0][0][0].Render();
}

void World::GenerateChunk(int const chunkX, int const chunkY, int const chunkZ) {
    Chunk& chunk = chunks[chunkX][chunkY][chunkZ];
    if (!chunk.isAllocated) {
        chunk.AllocateChunk();
        chunk.GenerateMesh(*this);

        //// Merge chunk's mesh into the world mesh
        //std::vector<GLfloat> chunkVertices = chunk.GetVertices();
        //std::vector<GLuint> chunkIndices = chunk.GetIndices();
        //int startIndex = (int)vertices.size() / 3; // Assuming vertices are interleaved positions and normals
        //int numVertices = (int)chunkVertices.size() / 3; // Adjust as per your vertex layout
        //int numIndices = (int)chunkIndices.size();
        //
        //// Merge vertices
        //vertices.insert(vertices.end(), chunkVertices.begin(), chunkVertices.end());
        //
        //// Adjust indices based on the offset of the merged vertices
        //for (GLuint& index : chunkIndices) {
        //    index += startIndex;
        //}
        //
        //// Merge indices
        //indices.insert(indices.end(), chunkIndices.begin(), chunkIndices.end());
        //
        //// Update start and end indices for the chunk
        //chunk.SetStartIndex(startIndex);
        //chunk.SetEndIndex(startIndex + numVertices - 1);
    }
    else {
        chunk.GenerateMesh(*this);
    }
}

World::~World() {

}