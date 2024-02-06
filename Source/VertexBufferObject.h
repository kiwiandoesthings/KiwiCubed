#pragma once

#include <glad/glad.h>

#include <iostream>

class VertexBufferObject
{
	public:
		GLuint vertexBufferObjectID;
		VertexBufferObject(GLfloat* verticies, GLsizeiptr size);

		void Bind();
		void Unbind();
		void Delete();
};