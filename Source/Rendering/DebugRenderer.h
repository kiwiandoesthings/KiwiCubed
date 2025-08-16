#pragma once

#include "glad/glad.h"

#include <vector>

#include "Shader.h"


class DebugRenderer {
	public:
		DebugRenderer() {};
		~DebugRenderer() {};

		void SetupBuffers(const std::vector<GLfloat>& chunkDebugVertices, const std::vector<GLuint>& chunkDebugIndices, const std::vector<glm::vec4>& chunkOrigins);
		void UpdateBuffers(const std::vector<GLfloat>& chunkDebugVertices, const std::vector<GLuint>& chunkDebugIndices, const std::vector<glm::vec4>& chunkOrigins);
		void UpdateUniforms() const;
		void RenderDebug(Shader& chunkDebugShaderProgram) const;

	private:
		GLuint chunkDebugVAO;
		GLuint chunkDebugVBO;
		GLuint chunkDebugIBO;
		GLuint chunkDebugIndexBO;

		std::vector<GLfloat> chunkDebugVertices;
		std::vector<GLuint> chunkDebugIndices;
		std::vector<glm::vec4> chunkOrigins;
};