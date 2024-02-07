#include <FastNoise/FastNoise.h>

#include <Block.h>


GLfloat faceVertices[] = {
	// Positions       // Texture Coordinates
	// Front
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
	// Front
	0, 1, 2,
	2, 3, 0,

	// Back
	4, 5, 6,
	6, 7, 4,

	// Left
	8, 9, 10,
	10, 11, 8,

	//Right
	12, 13, 14,
	14, 15, 12,

	//Top
	16, 17, 18,
	18, 19, 16,

	//Bottom
	20, 21, 22,
	22, 23, 20
};

Block::Block(int blockX, int blockY, int blockZ) : blockX(blockX), blockY(blockY), blockZ(blockZ) {
}

void Block::GenerateBlock(int blockX, int blockY, int blockZ, int chunkX, int chunkY, int chunkZ, int chunkSize) {
	if (GetSolid() == -1) {
		// Generate terrain heights using FastNoiseLite
		FastNoiseLite noise;
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);


		// Adjust the scale according to your terrain preferences
		float noiseValue = noise.GetNoise((float)blockX + (chunkX * chunkSize), (float)blockZ + (chunkZ * chunkSize));

		int terrainHeight = static_cast<int>(noiseValue * chunkSize);

		// Check if the block is solid based on terrain height
		if (blockY <= terrainHeight) {
			SetSolid(1);
		}
		else {
			SetSolid(0);
		}
	}
}

void Block::SetSolid(int solid) {
	isSolid = solid;
}

int Block::GetSolid() {
	return isSolid;
}

void Block::GenerateMesh(int x, int y, int z, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, FaceDirection faceDirection) {
	AddFace(vertices, indices, x, y, z, faceDirection);
}

void Block::AddFace(std::vector<GLfloat>&vertices, std::vector<GLuint>&indices, int x, int y, int z, FaceDirection faceDirection) { 
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