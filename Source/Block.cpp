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
	 	   		
	// Righ		
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


Block::Block(int type) : blockX(0), blockY(0), blockZ(0), type(NULL) {

}

void Block::GenerateBlock(int newBlockX, int newBlockY, int newBlockZ, int chunkX, int chunkY, int chunkZ, int chunkSize) {
	if (GetType() == NULL) {
		blockX = newBlockX;
		blockY = newBlockY;
		blockZ = newBlockZ;

		// Setup FastNoiseLite
		FastNoiseLite noise;
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noise.SetSeed(120);

		//float noiseValue = noise.GetNoise((float)blockX + (chunkX * chunkSize), (float)blockY + (chunkY * chunkSize), (float)blockZ + (chunkZ * chunkSize));
		//type = (noiseValue > 0) ? 1 : 0;
		type = 1;
	}
}

void Block::AddFace(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, FaceDirection faceDirection, int chunkX, int chunkY, int chunkZ, int chunkSize) {
	size_t vertexOffset = (size_t)faceDirection * 20;
	size_t baseIndex = vertices.size() / 5;

	for (size_t i = vertexOffset; i < vertexOffset + 20; i += 5) {
		vertices.emplace_back(faceVertices[i] +      (GLuint)blockX + (chunkX * chunkSize));
		vertices.emplace_back(faceVertices[i  + 1] + (GLuint)blockY + (chunkY * chunkSize));
		vertices.emplace_back(faceVertices[i  + 2] + (GLuint)blockZ + (chunkZ * chunkSize));
		vertices.emplace_back(faceVertices[i  + 3]);
		vertices.emplace_back(faceVertices[i  + 4]);
	}

	for (size_t i = 0; i < 6; ++i) {
		indices.emplace_back(baseIndex + faceIndices[i]);
	}
}
bool Block::GetType() {
	return type;
}

void Block::SetType(bool newType) {
	type = newType;
}