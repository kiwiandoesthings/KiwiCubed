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


void Block::GenerateBlock(BlockPos newpos, int chunkX, int chunkY, int chunkZ, unsigned int chunkSize, bool debug) {
	pos = newpos;

	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	noise.SetSeed(120);

	int newChunkSize = static_cast<int>(chunkSize);

	float density = noise.GetNoise(
		static_cast<float>(static_cast<int>(pos.x) + (chunkX * newChunkSize)),
		static_cast<float>(static_cast<int>(pos.y) + (chunkY * newChunkSize)),
		static_cast<float>(static_cast<int>(pos.z) + (chunkZ * newChunkSize))
	);

	if (density > 0) {
		type = gBlockManager.GetBlockType("kiwicubed", "stone");
		variant = rand() % 4;
	} else {
		type = gBlockManager.GetBlockType("kiwicubed", "air");
		variant = 0;
	}
}

void Block::AddFace(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, FaceDirection faceDirection, int chunkX, int chunkY, int chunkZ, unsigned int chunkSize) {
	GLuint vertexOffset = static_cast<GLuint>(faceDirection) * 20;
	GLuint baseIndex = static_cast<GLuint>(vertices.size() / 6);

	for (size_t i = vertexOffset; i < static_cast<size_t>(vertexOffset) + 20; i += 5) {
		vertices.emplace_back(faceVertices[i + 0] + static_cast<GLfloat>(pos.x + (chunkX * static_cast<int>(chunkSize))));
		vertices.emplace_back(faceVertices[i + 1] + static_cast<GLfloat>(pos.y + (chunkY * static_cast<int>(chunkSize))));
		vertices.emplace_back(faceVertices[i + 2] + static_cast<GLfloat>(pos.z + (chunkZ * static_cast<int>(chunkSize))));
		vertices.emplace_back(faceVertices[i + 3]);
		vertices.emplace_back(faceVertices[i + 4]);
		vertices.emplace_back(static_cast<GLfloat>(type->textures[variant]));
	}
	
	for (size_t i = 0; i < 6; ++i) {
		indices.emplace_back(static_cast<GLuint>(baseIndex + faceIndices[i]));
	}
}
