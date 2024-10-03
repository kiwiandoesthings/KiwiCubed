#pragma once

#include "GLError.h"
#include <glad/glad.h>

#include <map>
#include <vector>

#include "Player.h"
#include "Shader.h"
#include "ChunkHandler.h"


class Chunk;

struct ChunkData {
	glm::vec3 position;
};

struct Position {
	int chunkX, chunkY, chunkZ;
};


class World {
	public:
		Player player = Player(0, 0, 0);

		World();
		~World();

		void Setup(Window& window);

		void Render(Shader shaderProgram);
		void GenerateWorld();
		void GenerateChunk(int chunkX, int chunkY, int chunkZ, Chunk& chunk, bool updateCallerChunk, Chunk& callerChunk);

	private:
		int worldSize = 5;
		int chunksize = 32;
		ChunkHandler chunkHandler;

		int totalChunks;
		float totalMemoryUsage;

		std::vector<GLfloat> vertices;
		std::vector<GLuint> indices;

		int latestChunkVertexOffset = 0;
		int latestChunkIndexOffset = 0;

		Renderer renderer;

		IndexBufferObject indexBufferObject;
		VertexArrayObject vertexArrayObject;
		VertexBufferObject vertexBufferObject;
};