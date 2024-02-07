#pragma once

#include <glad/glad.h>

#include <iostream>

class IndexBufferObject
{
	public:
		GLuint indexBufferObjectID;
		IndexBufferObject();

		void Setup(GLsizeiptr size, GLuint* indicies);
		void Bind();
		void Unbind();
		void Delete();
};