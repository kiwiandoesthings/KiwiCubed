#pragma once

#include <glad/glad.h>

#include <iostream>

class IndexBufferObject
{
	public:
		GLuint indexBufferObjectID;
		IndexBufferObject(GLuint* indicies, GLsizeiptr size);

		void Bind();
		void Unbind();
		void Delete();
};