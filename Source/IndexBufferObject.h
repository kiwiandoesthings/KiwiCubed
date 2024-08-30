#pragma once

#include "GLError.h"

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