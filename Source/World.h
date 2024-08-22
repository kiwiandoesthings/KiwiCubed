#pragma once

#include "GLError.h"
#include <glad/glad.h>

#include <map>
#include <vector>

#include "Player.h"
#include "Renderer.h"
#include "Shader.h"
#include "ChunkHandler.h"


class Chunk;

struct ChunkData {
	glm::vec3 position;
	// Future other data
};

struct Position {
	int chunkX, chunkY, chunkZ;
};


class World {
	public:
		Player player = Player(0, 0, 0);

		World();
		~World();

		void Render(Shader shaderProgram);
		void GenerateWorld();
		void GenerateChunk(int chunkX, int chunkY, int chunkZ, Chunk& chunk, bool updateCallerChunk, Chunk& callerChunk);

	private:
		const int worldSize = 5;
		const int chunksize = 32;
		ChunkHandler chunkHandler;

		int totalChunks;
		float totalMemoryUsage;

		std::vector<Position> chunkLocations;

		std::vector<GLfloat> vertices;
		std::vector<GLuint> indices;

		GLuint indirectBufferObject;
		GLuint chunkDataBufferObject;

		std::vector<ChunkData> chunkData;

		int latestChunkVertexOffset = 0;
		int latestChunkIndexOffset = 0;

		Renderer renderer;

		IndexBufferObject indexBufferObject;
		VertexArrayObject vertexArrayObject;
		VertexBufferObject vertexBufferObject;
};