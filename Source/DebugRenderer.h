#pragma once

#include "GLError.h"
#include "glad/glad.h"

#include <vector>

#include "glm/vec3.hpp"

#include "Shader.h"


class DebugRenderer {
	public:
		DebugRenderer();
		~DebugRenderer();

		void SetupBuffers(const glm::vec3& playerPhysicsBoundingBoxCorner1, const glm::vec3& playerPhysicsBoundingBoxCorner2, const glm::vec3& playerPosition);
		void UpdateBuffers(const glm::vec3& playerPhysicsBoundingBoxCorner1, const glm::vec3& playerPhysicsBoundingBoxCorner2, const glm::vec3& playerPosition) const;
		void RenderDebug(Shader& wireframeShaderProgram, Shader& chunkDebugShaderProgram) const;

	private:
		GLuint playerPhysicsBoundingBoxVAO;
		GLuint playerPhysicsBoundingBoxVBO;
		GLuint playerPhysicsBoundingBoxEBO;

		GLuint playerPhysicsBoundingBoxIndices;
		std::vector<glm::vec3> playerPhysicsBoundingBoxVertices;

		GLuint chunkDebugVAO;
		GLuint chunkDebugVBO;
		GLuint chunkDebugEBO;

		GLuint chunkDebugIndices;
		std::vector<glm::vec3> chunkDebugVertices;
};