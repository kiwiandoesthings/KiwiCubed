#include "DebugRenderer.h"

DebugRenderer::DebugRenderer() {
}

// I have so little fucking idea how to make this more modular. I feel it's going to be ultra hardcoded for a very long time.
void DebugRenderer::SetupBuffers(const glm::vec3& playerPhysicsBoundingBoxCorner1, const glm::vec3& playerPhysicsBoundingBoxCorner2, const glm::vec3& playerPosition, const std::vector<glm::vec3>& chunkOrigins) {
	// PlayerPhysicsBoundingBox setup

	std::vector<glm::vec3> playerPhysicsBoundingBoxVertices = {
		{playerPhysicsBoundingBoxCorner1.x + playerPosition.x, playerPhysicsBoundingBoxCorner1.y + playerPosition.y, playerPhysicsBoundingBoxCorner1.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner2.x + playerPosition.x, playerPhysicsBoundingBoxCorner1.y + playerPosition.y, playerPhysicsBoundingBoxCorner1.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner2.x + playerPosition.x, playerPhysicsBoundingBoxCorner2.y + playerPosition.y, playerPhysicsBoundingBoxCorner1.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner1.x + playerPosition.x, playerPhysicsBoundingBoxCorner2.y + playerPosition.y, playerPhysicsBoundingBoxCorner1.z + playerPosition.z},
		{playerPhysicsBoundingBoxCorner1.x + playerPosition.x, playerPhysicsBoundingBoxCorner1.y + playerPosition.y, playerPhysicsBoundingBoxCorner2.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner2.x + playerPosition.x, playerPhysicsBoundingBoxCorner1.y + playerPosition.y, playerPhysicsBoundingBoxCorner2.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner2.x + playerPosition.x, playerPhysicsBoundingBoxCorner2.y + playerPosition.y, playerPhysicsBoundingBoxCorner2.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner1.x + playerPosition.x, playerPhysicsBoundingBoxCorner2.y + playerPosition.y, playerPhysicsBoundingBoxCorner2.z + playerPosition.z}
	};

	GLuint playerPhysicsBoundingBoxIndices[] = {
			0, 1, 1, 2, 2, 3, 3, 0,
			4, 5, 5, 6, 6, 7, 7, 4,
			0, 4, 1, 5, 2, 6, 3, 7
	};


	GLCall(glGenVertexArrays(1, &playerPhysicsBoundingBoxVAO));
	GLCall(glGenBuffers(1, &playerPhysicsBoundingBoxVBO));
	GLCall(glGenBuffers(1, &playerPhysicsBoundingBoxEBO));

	GLCall(glBindVertexArray(playerPhysicsBoundingBoxVAO));

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, playerPhysicsBoundingBoxVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * playerPhysicsBoundingBoxVertices.size(), playerPhysicsBoundingBoxVertices.data(), GL_STATIC_DRAW));

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerPhysicsBoundingBoxEBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(playerPhysicsBoundingBoxIndices), playerPhysicsBoundingBoxIndices, GL_STATIC_DRAW));

	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0));
	GLCall(glEnableVertexAttribArray(0));


	// Chunk debug setup

	DebugRenderer::chunkOrigins = chunkOrigins;
	
	std::vector<float> chunkDebugVertices = {
    	-0.5f, -0.5f, 0.0f,
    	 0.5f, -0.5f, 0.0f,
    	 0.5f,  0.5f, 0.0f,
    	-0.5f,  0.5f, 0.0f
	};

	GLuint chunkDebugIndices[] = {
    	0, 1, 2,
    	0, 2, 3
	};


	GLCall(glGenVertexArrays(1, &chunkDebugVAO));
	GLCall(glGenBuffers(1, &chunkDebugVBO));
	GLCall(glGenBuffers(1, &chunkDebugIBO));
	GLCall(glGenBuffers(1, &chunkDebugIndexBO));

	GLCall(glBindVertexArray(chunkDebugVAO));

	// Regular data
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, chunkDebugVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * chunkDebugVertices.size(), chunkDebugVertices.data(), GL_STATIC_DRAW));

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkDebugIBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(chunkDebugIndices), chunkDebugIndices, GL_STATIC_DRAW));

	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));
	GLCall(glEnableVertexAttribArray(0));

	// Instance data
	glBindBuffer(GL_ARRAY_BUFFER, chunkDebugIndexBO);
	glBufferData(GL_ARRAY_BUFFER, chunkOrigins.size() * sizeof(glm::vec3), &chunkOrigins[0], GL_STATIC_DRAW);

	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	glVertexAttribDivisor(2, 1);

	// Future setups go here
}

void DebugRenderer::UpdateBuffers(const glm::vec3& playerPhysicsBoundingBoxCorner1, const glm::vec3& playerPhysicsBoundingBoxCorner2, const glm::vec3& playerPosition) const {
	// PlayerPhysicsBoundingBox buffers
	GLuint playerPhysicsBoundingBoxIndices[] = {
			0, 1, 1, 2, 2, 3, 3, 0,
			4, 5, 5, 6, 6, 7, 7, 4,
			0, 4, 1, 5, 2, 6, 3, 7
	};

	std::vector<glm::vec3> playerPhysicsBoundingBoxVertices = {
		{playerPhysicsBoundingBoxCorner1.x + playerPosition.x, playerPhysicsBoundingBoxCorner1.y + playerPosition.y, playerPhysicsBoundingBoxCorner1.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner2.x + playerPosition.x, playerPhysicsBoundingBoxCorner1.y + playerPosition.y, playerPhysicsBoundingBoxCorner1.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner2.x + playerPosition.x, playerPhysicsBoundingBoxCorner2.y + playerPosition.y, playerPhysicsBoundingBoxCorner1.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner1.x + playerPosition.x, playerPhysicsBoundingBoxCorner2.y + playerPosition.y, playerPhysicsBoundingBoxCorner1.z + playerPosition.z},
		{playerPhysicsBoundingBoxCorner1.x + playerPosition.x, playerPhysicsBoundingBoxCorner1.y + playerPosition.y, playerPhysicsBoundingBoxCorner2.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner2.x + playerPosition.x, playerPhysicsBoundingBoxCorner1.y + playerPosition.y, playerPhysicsBoundingBoxCorner2.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner2.x + playerPosition.x, playerPhysicsBoundingBoxCorner2.y + playerPosition.y, playerPhysicsBoundingBoxCorner2.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner1.x + playerPosition.x, playerPhysicsBoundingBoxCorner2.y + playerPosition.y, playerPhysicsBoundingBoxCorner2.z + playerPosition.z}
	};

	GLCall(glBindVertexArray(playerPhysicsBoundingBoxVAO));

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, playerPhysicsBoundingBoxVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * playerPhysicsBoundingBoxVertices.size(), playerPhysicsBoundingBoxVertices.data(), GL_STATIC_DRAW));

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerPhysicsBoundingBoxEBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(playerPhysicsBoundingBoxIndices), playerPhysicsBoundingBoxIndices, GL_STATIC_DRAW));


	// Chunk debug buffers

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, chunkDebugIndexBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, chunkOrigins.size() * sizeof(glm::vec3), &chunkOrigins[0], GL_STATIC_DRAW));


	// Future updates go here
}

void DebugRenderer::UpdateUniforms() const {
}

void DebugRenderer::RenderDebug(Shader& wireframeShaderProgram, Shader& chunkDebugShaderProgram) const {
	// PlayerPhysicsBoundingBox renderer
	wireframeShaderProgram.Bind();
	GLCall(glBindVertexArray(playerPhysicsBoundingBoxVAO));
	GLCall(glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0));
	GLCall(glBindVertexArray(0));

	
	// Chunk debug renderer
	chunkDebugShaderProgram.Bind();
	GLCall(glBindVertexArray(chunkDebugVAO));
	GLCall(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, chunkOrigins.size()););
	GLCall(glBindVertexArray(0));


	// Future renderers go here
}

DebugRenderer::~DebugRenderer() {
}