#pragma once

#include <glad/glad.h>
#include <GL/gl.h>

#include <vector>

#include <Chunk.h>
#include <Player.h>
#include <Shader.h>


class World {
	public:
		World();
		~World();

		void Render();
		void GenerateChunk(int const chunkX, int const chunkY, int const chunkZ);

		Player player = Player(0, 0, 0);

	private:
		int worldSize = 2;
		Chunk*** chunks;

		std::vector<GLfloat> vertices;
		std::vector<GLuint> indices;

		IndexBufferObject indexBufferObject;
		VertexArrayObject vertexArrayObject;
		VertexBufferObject vertexBufferObject;
};