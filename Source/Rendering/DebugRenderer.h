#pragma once

#include "glad/glad.h"

#include <vector>

#include "Shader.h"


class DebugRenderer {
	public:
		DebugRenderer() {};
		~DebugRenderer() {};

		void SetupBuffers(const glm::vec3& playerPhysicsBoundingBoxCorner1, const glm::vec3& playerPhysicsBoundingBoxCorner2, const glm::vec3& playerPosition, const std::vector<GLfloat>& chunkDebugVertices, const std::vector<GLuint>& chunkDebugIndices, const std::vector<glm::vec4>& chunkOrigins);
		void UpdateBuffers(const glm::vec3& playerPhysicsBoundingBoxCorner1e, const glm::vec3& playerPhysicsBoundingBoxCorner2, const glm::vec3& playerPosition, const std::vector<GLfloat>& chunkDebugVertices, const std::vector<GLuint>& chunkDebugIndices, const std::vector<glm::vec4>& chunkOrigins);
		void UpdateUniforms() const;
		void RenderDebug(Shader& wireframeShaderProgram, Shader& chunkDebugShaderProgram) const;

	private:
		GLuint playerPhysicsBoundingBoxVAO;
		GLuint playerPhysicsBoundingBoxVBO;
		GLuint playerPhysicsBoundingBoxEBO;

		std::vector<glm::vec3> playerPhysicsBoundingBoxVertices;
		GLuint playerPhysicsBoundingBoxIndices;

		GLuint chunkDebugVAO;
		GLuint chunkDebugVBO;
		GLuint chunkDebugIBO;
		GLuint chunkDebugIndexBO;

		std::vector<GLfloat> chunkDebugVertices;
		std::vector<GLuint> chunkDebugIndices;
		std::vector<glm::vec4> chunkOrigins;
};