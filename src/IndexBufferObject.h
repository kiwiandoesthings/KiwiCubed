#pragma once

#include "GLError.h"

#include <iostream>

class IndexBufferObject
{
	public:
		GLuint indexBufferObjectID;

		IndexBufferObject();

		int SetupBuffer();
		void SetBufferData(GLsizeiptr size, GLuint* indicies);
		void Bind() const;
		void Unbind();
		void Delete() const;
};