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


void Block::GenerateBlock(unsigned short blockX, unsigned short blockY, unsigned short blockZ, int chunkX, int chunkY, int chunkZ, unsigned int chunkSize, bool debug) {
	Block::blockX = blockX;
	Block::blockY = blockY;
	Block::blockZ = blockZ;

	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	noise.SetSeed(120);

	int newChunkSize = static_cast<int>(chunkSize);

	float density = noise.GetNoise(static_cast<float>(blockX + (chunkX * newChunkSize)), static_cast<float>(blockZ + (chunkZ * newChunkSize)));
	if (blockY + (chunkY * newChunkSize) < (density + 1) * 30) {
		blockID = static_cast<unsigned short>(*textureManager.GetNumericalID({"kiwicubed", "stone"}));
		variant = rand() % textureManager.GetTextureAtlasData(blockID)->size();
	} else {
		blockID = 0;
		variant = 0;
	}
}

void Block::AddFaces(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, std::vector<FaceDirection>* faceDirections, int chunkX, int chunkY, int chunkZ, unsigned int chunkSize, TextureAtlasData& atlasData) {
	for (unsigned int iterator = 0; iterator < faceDirections->size(); ++iterator) {
		GLuint vertexOffset = static_cast<GLuint>(faceDirections->at(iterator)) * 20;
		GLuint baseIndex = static_cast<GLuint>(vertices.size() / 5);

		for (size_t i = vertexOffset; i < vertexOffset + 20; i += 5) {
			vertices.emplace_back((faceVertices[i + 0]) + static_cast<GLfloat>(blockX + (chunkX * static_cast<int>(chunkSize))));
			vertices.emplace_back((faceVertices[i + 1]) + static_cast<GLfloat>(blockY + (chunkY * static_cast<int>(chunkSize))));
			vertices.emplace_back((faceVertices[i + 2]) + static_cast<GLfloat>(blockZ + (chunkZ * static_cast<int>(chunkSize))));

			switch ((i - vertexOffset) / 5 % 4) {
				case 0: {
					float u0 = atlasData.xPosition / 3.0f; 
					float v1 = (atlasData.yPosition + atlasData.ySize) / 3.0f; 
					vertices.emplace_back(u0);
					vertices.emplace_back(v1);
					break;
				}
				case 1: {
					float u1 = (atlasData.xPosition + atlasData.xSize) / 3.0f; 
					float v1 = (atlasData.yPosition + atlasData.ySize) / 3.0f;
					vertices.emplace_back(u1);
					vertices.emplace_back(v1);
					break;
				}
				case 2: {
					float u1 = (atlasData.xPosition + atlasData.xSize) / 3.0f; 
					float v0 = atlasData.yPosition / 3.0f; 
					vertices.emplace_back(u1);
					vertices.emplace_back(v0);
					break;
				}
				case 3: {
					float u0 = atlasData.xPosition / 3.0f; 
					float v0 = atlasData.yPosition / 3.0f; 
					vertices.emplace_back(u0);
					vertices.emplace_back(v0);
					break;
				}
			}
		}

		for (size_t i = 0; i < 6; ++i) {
			indices.emplace_back(static_cast<GLuint>(baseIndex + faceIndices[i]));
		}
	}
}
	
unsigned int Block::GetBlockID() const {
	return blockID;
}

void Block::SetBlockID(unsigned int newBlockID) {
	blockID = newBlockID;
}

unsigned short Block::GetVariant() const {
	return variant;
}

void Block::SetVariant(unsigned short newVariant) {
	variant = newVariant;
}

bool Block::IsAir() {
	if (blockID == 0) {
		return true;
	} else {
		return false;
	}
}