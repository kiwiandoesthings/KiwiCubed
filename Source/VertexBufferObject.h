#pragma once

#include <GLError.h>

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