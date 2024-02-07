#pragma once

#include <glad/glad.h>

#include <iostream>
#include <vector>

class VertexBufferObject
{
	public:
		GLuint vertexBufferObjectID;
		VertexBufferObject();
		
		void Setup(GLsizeiptr size, GLfloat* vertices);
		void Bind();
		void Unbind();
		void Delete();
};