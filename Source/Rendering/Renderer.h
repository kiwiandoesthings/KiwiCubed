#pragma once

#include <glad/glad.h>

#include "IndexBufferObject.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

struct Vertex {
	GLfloat position[3];
	GLfloat textureCoordinate[2];
	GLuint textureIndex;
};


class Renderer {
	public:
		Renderer() {};

		void ClearScreen(float redValue, float blueValue, float greenValue);
		void DrawElements(VertexArrayObject vertexArrayObject, VertexBufferObject vertexBufferObject, IndexBufferObject indexBufferObject, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices);

		void Delete() {};
};