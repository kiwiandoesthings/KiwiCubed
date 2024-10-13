#include "DebugRenderer.h"

DebugRenderer::DebugRenderer() {
}

// I have so little fucking idea how to make this more modular. I feel it's going to be ultra hardcoded for a very long time.
void DebugRenderer::SetupBuffers(glm::vec3 playerPhysicsBoundingBoxCorner1, glm::vec3 playerPhysicsBoundingBoxCorner2, glm::vec3 playerPosition) {
	// PlayerPhysicsBoundingBox setup

	//GLuint playerPhysicsBoundingBoxindices[] = {
	//		0, 1, 1, 2, 2, 3, 3, 0,
	//		4, 5, 5, 6, 6, 7, 7, 4,
	//		0, 4, 1, 5, 2, 6, 3, 7
	//};
	//
	//glm::vec3 c1 = playerPhysicsBoundingBoxCorner1;
	//glm::vec3 c2 = playerPhysicsBoundingBoxCorner2;
	//glm::vec3 pos = playerPosition;
	//
	//std::vector<glm::vec3> playerPhysicsBoundingBoxVertices = {
	//		{c1.x + pos.x, c1.y + pos.y, c1.z + pos.z}, {c2.x + pos.x, c1.y + pos.y, c1.z + pos.z}, {c2.x + pos.x, c2.y + pos.y, c1.z + pos.z}, {c1.x + pos.x, c2.y + pos.y, c1.z + pos.z},
	//		{c1.x + pos.x, c1.y + pos.y, c2.z + pos.z}, {c2.x + pos.x, c1.y + pos.y, c2.z + pos.z}, {c2.x + pos.x, c2.y + pos.y, c2.z + pos.z}, {c1.x + pos.x, c2.y + pos.y, c2.z + pos.z}
	//};
	//
	//glGenVertexArrays(1, &playerPhysicsBoundingBoxVAO);
	//glGenBuffers(1, &playerPhysicsBoundingBoxVBO);
	//glGenBuffers(1, &playerPhysicsBoundingBoxEBO);
	//
	//glBindVertexArray(playerPhysicsBoundingBoxVAO);
	//
	//glBindBuffer(GL_ARRAY_BUFFER, playerPhysicsBoundingBoxVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * playerPhysicsBoundingBoxVertices.size(), playerPhysicsBoundingBoxVertices.data(), GL_STATIC_DRAW);
	//
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerPhysicsBoundingBoxEBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(playerPhysicsBoundingBoxIndices), playerPhysicsBoundingBoxIndices, GL_STATIC_DRAW);
	//
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	//glEnableVertexAttribArray(0);

	std::cout << "stfu" << std::endl;

	// Future setups go here
}

void DebugRenderer::RenderDebug(Shader& wireframeShaderProgram) {
	// PlayerPhysicsBoundingBox renderer
	//wireframeShaderProgram.Bind();
	//glBindVertexArray(playerPhysicsBoundingBoxVAO);
	//glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);

	std::cout << "honest to fuck" << std::endl;

	// Future renderers go here
}

DebugRenderer::~DebugRenderer() {
}