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
	0.0f, 1.0f, 1.0f,  1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
	1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,  1.0f, 0.0f,

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


Block::Block(int type) : blockX(0), blockY(0), blockZ(0), type(-1) {

}

void Block::GenerateBlock(int newBlockX, int newBlockY, int newBlockZ, int chunkX, int chunkY, int chunkZ, int chunkSize) {
	blockX = newBlockX;
	blockY = newBlockY;
	blockZ = newBlockZ;
	if (GetType() == -1) {
		// Setup FastNoiseLite
		FastNoiseLite noise;
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noise.SetSeed(120);

		// Get the noise value for the block
		float noiseValue = noise.GetNoise((float)blockX + (chunkX * chunkSize), (float)blockY + (chunkY * chunkSize), (float)blockZ + (chunkZ * chunkSize));
		if (noiseValue > 0) {
			SetType(1);
		}
		else {
			SetType(0);
		}
		//if (blockX == 0 && blockY == 0 && blockZ == 0) {
		//	SetType(1);
		//}
		//else {
		//	SetType(0);
		//}
		//SetType(1);
	}
}

void Block::AddFace(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, FaceDirection faceDirection, int chunkX, int chunkY, int chunkZ, int chunkSize) {
	// Calculate offset based on block position
	GLfloat xOffset = static_cast<GLfloat>(blockX + (chunkX * chunkSize));
	GLfloat yOffset = static_cast<GLfloat>(blockY + (chunkY * chunkSize));
	GLfloat zOffset = static_cast<GLfloat>(blockZ + (chunkZ * chunkSize));
	size_t vertexOffset = static_cast<size_t>(faceDirection) * 20;

	// Append face vertices with offset to the chunk's vertex array
	for (size_t i = vertexOffset; i < vertexOffset + 20; i += 5) {
		vertices.push_back(faceVertices[i] + xOffset);
		vertices.push_back(faceVertices[i + 1] + yOffset);
		vertices.push_back(faceVertices[i + 2] + zOffset);
		vertices.push_back(faceVertices[i + 3]); // Texture coordinate s
		vertices.push_back(faceVertices[i + 4]); // Texture coordinate t
	}

	// Calculate indices for the face
	GLuint baseIndex = static_cast<GLuint>(vertices.size()) / vertexSize - 4;

	// Push all indices for the face
	for (size_t i = 0; i < 6; ++i) {
		indices.push_back(baseIndex + faceIndices[i]);
	}
}

int Block::GetType() {
	return type;
}

void Block::SetType(int newType) {
	type = newType;
}