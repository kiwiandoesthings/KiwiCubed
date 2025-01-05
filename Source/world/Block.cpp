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
	0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,  1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
	0.0f, 0.0f, 0.0f,  0.0f, 0.0f
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


void Block::GenerateBlock(BlockPosition newBlockPosition, int chunkX, int chunkY, int chunkZ, unsigned int chunkSize, bool debug) {
	blockPosition = newBlockPosition;

	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	noise.SetSeed(120);

	int newChunkSize = static_cast<int>(chunkSize);

	float density = noise.GetNoise(
		static_cast<float>(blockPosition.xInt() + (chunkX * newChunkSize)),
		static_cast<float>(blockPosition.yInt() + (chunkY * newChunkSize)),
		static_cast<float>(blockPosition.zInt() + (chunkZ * newChunkSize))
	);

	if (density > 0) {
		blockID = *blockManager.GetBlockID("kiwicubed", "stone");
		variant = rand() % 4;
	} else {
		blockID = *blockManager.GetBlockID("kiwicubed", "air");
		variant = 0;
	}
}

void Block::AddFace(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, FaceDirection faceDirection, int chunkX, int chunkY, int chunkZ, unsigned int chunkSize) {
	GLuint vertexOffset = static_cast<GLuint>(faceDirection) * 20;
	GLuint baseIndex = static_cast<GLuint>(vertices.size() / 6);

	for (size_t i = vertexOffset; i < static_cast<size_t>(vertexOffset) + 20; i += 5) {
		vertices.emplace_back(faceVertices[i + 0] + static_cast<GLfloat>(blockPosition.xInt() + (chunkX * static_cast<int>(chunkSize))));
		vertices.emplace_back(faceVertices[i + 1] + static_cast<GLfloat>(blockPosition.yInt() + (chunkY * static_cast<int>(chunkSize))));
		vertices.emplace_back(faceVertices[i + 2] + static_cast<GLfloat>(blockPosition.zInt() + (chunkZ * static_cast<int>(chunkSize))));
		vertices.emplace_back(faceVertices[i + 3]);
		vertices.emplace_back(faceVertices[i + 4]);
		vertices.emplace_back(static_cast<GLfloat>(blockManager.GetBlockType(blockID)->textures[variant]));
	}
	
	for (size_t i = 0; i < 6; ++i) {
		indices.emplace_back(static_cast<GLuint>(baseIndex + faceIndices[i]));
	}
}

unsigned int Block::GetBlockID() const {
	return blockID;
}
void Block::SetBlockID(unsigned short newBlockID) {
	blockID = newBlockID;
}

bool Block::IsAir() {
	return blockID == 0;
}