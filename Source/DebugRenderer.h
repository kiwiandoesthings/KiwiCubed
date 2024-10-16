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

		void SetupBuffers(glm::vec3 playerPhysicsBoundingBoxCorner1, glm::vec3 playerPhysicsBoundingBoxCorner2, glm::vec3 playerPosition);
		void RenderDebug(Shader& wireframeShaderProgram);

	private:
		GLuint playerPhysicsBoundingBoxVAO;
		GLuint playerPhysicsBoundingBoxVBO;
		GLuint playerPhysicsBoundingBoxEBO;

		GLuint playerPhysicsBoundingBoxIndices;
		std::vector<glm::vec3> playerPhysicsBoundingBoxVertices;
};