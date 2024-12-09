#include "DebugRenderer.h"

DebugRenderer::DebugRenderer() {
}

// I have so little fucking idea how to make this more modular. I feel it's going to be ultra hardcoded for a very long time.
void DebugRenderer::SetupBuffers(const glm::vec3& playerPhysicsBoundingBoxCorner1, const glm::vec3& playerPhysicsBoundingBoxCorner2, const glm::vec3& playerPosition) {
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


	glGenVertexArrays(1, &playerPhysicsBoundingBoxVAO);
	glGenBuffers(1, &playerPhysicsBoundingBoxVBO);
	glGenBuffers(1, &playerPhysicsBoundingBoxEBO);

	glBindVertexArray(playerPhysicsBoundingBoxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, playerPhysicsBoundingBoxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * playerPhysicsBoundingBoxVertices.size(), playerPhysicsBoundingBoxVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerPhysicsBoundingBoxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(playerPhysicsBoundingBoxIndices), playerPhysicsBoundingBoxIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);


	// Chunk debug setup
	
	std::vector<float> chunkDebugVertices = {
    	-0.5f, -0.5f, 0.0f,
     	0.5f, -0.5f, 0.0f,
     	0.5f,  0.5f, 0.0f,
    	-0.5f,  0.5f, 0.0f
	};
	GLuint chunkDebugIndices[] = {
    	0, 1, 2,
    	2, 3, 0 
	};


	glGenVertexArrays(1, &chunkDebugVAO);
	glGenBuffers(1, &chunkDebugVBO);
	glGenBuffers(1, &chunkDebugEBO);

	glBindVertexArray(chunkDebugVAO);

	glBindBuffer(GL_ARRAY_BUFFER, chunkDebugVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * chunkDebugVertices.size(), chunkDebugVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkDebugEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(chunkDebugIndices), chunkDebugIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	// Future setups go here
}

void DebugRenderer::UpdateBuffers(const glm::vec3& playerPhysicsBoundingBoxCorner1, const glm::vec3& playerPhysicsBoundingBoxCorner2, const glm::vec3& playerPosition) const {
	GLuint playerPhysicsBoundingBoxIndices[] = {
			0, 1, 1, 2, 2, 3, 3, 0,
			4, 5, 5, 6, 6, 7, 7, 4,
			0, 4, 1, 5, 2, 6, 3, 7
	};

	std::vector<glm::vec3> playerPhysicsBoundingBoxVertices = {
		{playerPhysicsBoundingBoxCorner1.x + playerPosition.x, playerPhysicsBoundingBoxCorner1.y + playerPosition.y, playerPhysicsBoundingBoxCorner1.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner2.x + playerPosition.x, playerPhysicsBoundingBoxCorner1.y + playerPosition.y, playerPhysicsBoundingBoxCorner1.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner2.x + playerPosition.x, playerPhysicsBoundingBoxCorner2.y + playerPosition.y, playerPhysicsBoundingBoxCorner1.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner1.x + playerPosition.x, playerPhysicsBoundingBoxCorner2.y + playerPosition.y, playerPhysicsBoundingBoxCorner1.z + playerPosition.z},
		{playerPhysicsBoundingBoxCorner1.x + playerPosition.x, playerPhysicsBoundingBoxCorner1.y + playerPosition.y, playerPhysicsBoundingBoxCorner2.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner2.x + playerPosition.x, playerPhysicsBoundingBoxCorner1.y + playerPosition.y, playerPhysicsBoundingBoxCorner2.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner2.x + playerPosition.x, playerPhysicsBoundingBoxCorner2.y + playerPosition.y, playerPhysicsBoundingBoxCorner2.z + playerPosition.z}, {playerPhysicsBoundingBoxCorner1.x + playerPosition.x, playerPhysicsBoundingBoxCorner2.y + playerPosition.y, playerPhysicsBoundingBoxCorner2.z + playerPosition.z}
	};

	glBindVertexArray(playerPhysicsBoundingBoxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, playerPhysicsBoundingBoxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * playerPhysicsBoundingBoxVertices.size(), playerPhysicsBoundingBoxVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerPhysicsBoundingBoxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(playerPhysicsBoundingBoxIndices), playerPhysicsBoundingBoxIndices, GL_STATIC_DRAW);
}

void DebugRenderer::RenderDebug(Shader& wireframeShaderProgram, Shader& chunkDebugShaderProgram) const {
	// PlayerPhysicsBoundingBox renderer
	wireframeShaderProgram.Bind();
	glBindVertexArray(playerPhysicsBoundingBoxVAO);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	
	// Chunk debug renderer
	chunkDebugShaderProgram.Bind();
	glBindVertexArray(chunkDebugVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);


	// Future renderers go here
}

DebugRenderer::~DebugRenderer() {
}