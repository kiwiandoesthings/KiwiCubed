#pragma once

#include <GLError.h>

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