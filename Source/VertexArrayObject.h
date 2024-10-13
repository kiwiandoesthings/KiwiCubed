#pragma once

#include <GLError.h>

#include <iostream>

#include <VertexBufferObject.h>

class VertexArrayObject
{
	public:
		GLuint vertexArrayObjectID;
		VertexArrayObject();

		void LinkAttribute(VertexBufferObject& vertexBufferObject, GLuint layout, GLuint componentCount, GLenum type, GLboolean isNormalized, GLsizeiptr stride, void* offset);
		void Bind() const;
		void Unbind();
		void Delete() const;
};