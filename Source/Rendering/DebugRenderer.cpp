#include "DebugRenderer.h"

// I have so little fucking idea how to make this more modular. I feel it's going to be ultra hardcoded for a very long time.
void DebugRenderer::SetupBuffers(const std::vector<GLfloat>& chunkDebugVertices, const std::vector<GLuint>& chunkDebugIndices, const std::vector<glm::vec4>& chunkOrigins) {
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

void DebugRenderer::UpdateBuffers(const std::vector<GLfloat>& chunkDebugVertices, const std::vector<GLuint>& chunkDebugIndices, const std::vector<glm::vec4>& chunkOrigins) {
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

void DebugRenderer::RenderDebug(Shader& chunkDebugShaderProgram) const {
	// Chunk debug renderer
	chunkDebugShaderProgram.Bind();
	GLCall(glBindVertexArray(chunkDebugVAO));
	GLCall(glDrawElementsInstanced(GL_TRIANGLES, chunkDebugIndices.size(), GL_UNSIGNED_INT, 0, chunkOrigins.size()););
	GLCall(glBindVertexArray(0));


	// Future renderers go here
}