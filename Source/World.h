#pragma once

#include <GLError.h>
#include <GLAD/glad.h>

#include <vector>

#include <Chunk.h>
#include <Player.h>
#include <Renderer.h>
#include <Shader.h>


typedef  struct {
	GLuint count;
	GLuint instanceCount;
	GLuint firstIndex;
	GLuint baseVertex;
	GLuint baseInstance;
} DrawElementsIndirectCommand;


class World {
public:
	Player player = Player(0, 0, 0);

	World();
	~World();

	void Render(Shader shaderProgram);
	void GenerateWorld();
	void GenerateChunk(int chunkX, int chunkY, int chunkZ);

private:
	const int worldSize = 2;
	const int chunksize = 32;
	Chunk*** chunks;

	int totalChunks;
	float totalMemoryUsage;

	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	GLuint indirectBuffer;

	int drawCount;
	std::vector<DrawElementsIndirectCommand> commands;
	size_t currentVertexOffset = 0;
	size_t currentIndexOffset = 0;

	int latestEndIndex = 0;

	Renderer renderer;

	IndexBufferObject indexBufferObject;
	VertexArrayObject vertexArrayObject;
	VertexBufferObject vertexBufferObject;
};