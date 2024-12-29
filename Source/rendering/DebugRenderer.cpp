#include "DebugRenderer.h"

// I have so little fucking idea how to make this more modular. I feel it's going to be ultra hardcoded for a very long time.
void DebugRenderer::SetupBuffers(const glm::vec3& playerPhysicsBoundingBoxCorner1, const glm::vec3& playerPhysicsBoundingBoxCorner2, const glm::vec3& playerPosition, const std::vector<GLfloat>& chunkDebugVertices, const std::vector<GLuint>& chunkDebugIndices, const std::vector<glm::vec4>& chunkOrigins) {
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
	
	DebugRenderer::chunkDebugVertices = chunkDebugVertices;
	DebugRenderer::chunkDebugIndices = chunkDebugIndices;


	GLCall(glGenVertexArrays(1, &chunkDebugVAO));
	GLCall(glGenBuffers(1, &chunkDebugVBO));
	GLCall(glGenBuffers(1, &chunkDebugIBO));
	GLCall(glGenBuffers(1, &chunkDebugIndexBO));

	GLCall(glBindVertexArray(chunkDebugVAO));

	// Regular data
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, chunkDebugVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * chunkDebugVertices.size(), chunkDebugVertices.data(), GL_STATIC_DRAW));

	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0));
	GLCall(glEnableVertexAttribArray(0));

	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3)));
	GLCall(glEnableVertexAttribArray(2));

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkDebugIBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * chunkDebugIndices.size(), chunkDebugIndices.data(), GL_STATIC_DRAW));

	// Instance data
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, chunkDebugIndexBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER,  sizeof(glm::vec4) * chunkOrigins.size(), chunkOrigins.data(), GL_STATIC_DRAW));

	GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribDivisor(1, 1));

	// Future setups go here
}

void DebugRenderer::UpdateBuffers(const glm::vec3& playerPhysicsBoundingBoxCorner1, const glm::vec3& playerPhysicsBoundingBoxCorner2, const glm::vec3& playerPosition, const std::vector<GLfloat>& chunkDebugVertices, const std::vector<GLuint>& chunkDebugIndices, const std::vector<glm::vec4>& chunkOrigins) {
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

	DebugRenderer::chunkOrigins = chunkOrigins;
	
	DebugRenderer::chunkDebugVertices = chunkDebugVertices;
	DebugRenderer::chunkDebugIndices = chunkDebugIndices;

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, chunkDebugVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * chunkDebugVertices.size(), chunkDebugVertices.data(), GL_STATIC_DRAW));

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkDebugIBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * chunkDebugIndices.size(), chunkDebugIndices.data(), GL_STATIC_DRAW));

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, chunkDebugIndexBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER,  sizeof(glm::vec4) * chunkOrigins.size(), chunkOrigins.data(), GL_STATIC_DRAW));


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
	GLCall(glDrawElementsInstanced(GL_TRIANGLES, chunkDebugIndices.size(), GL_UNSIGNED_INT, 0, chunkOrigins.size()););
	GLCall(glBindVertexArray(0));


	// Future renderers go here
}