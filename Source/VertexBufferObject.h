#pragma once

#include <GLError.h>

#include <iostream>
#include <vector>

class VertexBufferObject
{
	public:
		GLuint vertexBufferObjectID;
		VertexBufferObject(/*const char* initializer*/);
		
		void Setup(GLsizeiptr size, GLfloat* vertices);
		void Bind() const;
		void Unbind();
		void Delete() const;
};