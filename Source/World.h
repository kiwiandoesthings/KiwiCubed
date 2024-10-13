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
		Player player = Player(32, 35, 32);

		World();

		void Setup(Window& window);

		void Render(Shader shaderProgram);
		void GenerateWorld();
		void GenerateChunk(int chunkX, int chunkY, int chunkZ, Chunk& chunk, bool updateCallerChunk, Chunk& callerChunk);

		void Update(Window* window);

		void DisplayImGui();

		void Delete();

	private:
		unsigned int worldSize = 5;

		ChunkHandler chunkHandler;
		Chunk emptyChunk = Chunk(0, 0, 0);

		unsigned int totalChunks;
		float totalMemoryUsage;

		std::vector<GLfloat> vertices;
		std::vector<GLuint> indices;

		unsigned int latestChunkVertexOffset = 0;
		unsigned int latestChunkIndexOffset = 0;

		Renderer renderer;

		IndexBufferObject indexBufferObject;
		VertexArrayObject vertexArrayObject;
		VertexBufferObject vertexBufferObject = VertexBufferObject(/*"world*/);
};