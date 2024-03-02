#include <FastNoise/FastNoise.h>

#include <Block.h>


GLfloat faceVertices[] = {
	// Positions       // Texture Coordinates
	// 
	//Front
	0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
	1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
	0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	
	// Back
	0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
	1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
	1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,  0.0f, 1.0f,

	// Left
	0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
	0.0f, 1.0f, 1.0f,  1.0f, 1.0f,
	0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
	0.0f, 0.0f, 0.0f,  0.0f, 0.0f,

	// Right
	1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,  0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

	// Top
	0.0f, 1.0f, 1.0f,  0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	0.0f, 1.0f, 0.0f,  0.0f, 1.0f,

	// Bottom
	0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
	1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
	1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
	0.0f, 0.0f, 0.0f,  0.0f, 1.0f
};

GLuint faceIndices[] = {
	0, 1, 2,  // Front
	2, 3, 0,

	4, 5, 6,  // Back
	6, 7, 4,

	8, 9, 10,  // Left
	10, 11, 8,

	12, 13, 14,  // Right
	14, 15, 12,

	16, 17, 18,  // Top
	18, 19, 16,

	20, 21, 22,  // Bottom
	22, 23, 20
};

Block::Block(int blockX, int blockY, int blockZ) : blockX(blockX), blockY(blockY), blockZ(blockZ), isSolid(-1) {
}

bool Block::IsSolid(int x, int y, int z, int CHUNK_SIZE) {
	// Generate terrain heights using FastNoiseLite
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);


		// Adjust the scale according to your terrain preferences
		float noiseValue = noise.GetNoise((float)blockX + (chunkX * chunkSize), (float)blockY + (chunkY * chunkSize), (float)blockZ + (chunkZ * chunkSize));

	int terrainHeight = static_cast<int>(noiseValue * CHUNK_SIZE);

		// Check if the block is solid based on terrain height
		if (noiseValue > 0) {
			SetSolid(1);
		}
		else {
			SetSolid(0);
		}
	}
}

void Block::SetSolid(bool solid) {
	isSolid = solid;
}

void Block::GenerateMesh(int x, int y, int z, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, FaceDirection faceDirection) {
	addFace(vertices, indices, x, y, z, faceDirection);
}

void Block::addFace(std::vector<GLfloat>&vertices, std::vector<GLuint>&indices, int x, int y, int z, FaceDirection faceDirection) { 
	// Calculate offset based on block position
	GLfloat xOffset = static_cast<GLfloat>(x);
	GLfloat yOffset = static_cast<GLfloat>(y);
	GLfloat zOffset = static_cast<GLfloat>(z);


	int vertexOffset = static_cast<int>(faceDirection) * (20 * sizeof(GLfloat)) / sizeof(GLfloat);
	// Append face vertices with offset to the chunk's vertex array
	for (int i = vertexOffset; i < sizeof(faceVertices) / sizeof(GLfloat) / 6 + vertexOffset; i += vertexSize) {
		vertices.push_back(faceVertices[i] + xOffset);
		vertices.push_back(faceVertices[i + 1] + yOffset);
		vertices.push_back(faceVertices[i + 2] + zOffset);
		vertices.push_back(faceVertices[i + 3]);  // Texture coordinate s
		vertices.push_back(faceVertices[i + 4]);  // Texture coordinate t
	}

	// Calculate indices for the face
	GLuint baseIndex = static_cast<GLuint>(vertices.size()) / vertexSize - (sizeof(faceVertices) / sizeof(GLfloat)) / vertexSize;

	for (GLuint i = 0; i < sizeof(faceIndices) / sizeof(GLuint); ++i) {
		indices.push_back(baseIndex + faceIndices[i]);
	}
}