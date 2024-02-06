#include <VertexArrayObject.h>

VertexArrayObject::VertexArrayObject()
{
	glGenVertexArrays(1, &vertexArrayObjectID);
	std::cout << "Vertex Array Object Setup / Info: Successfully created vertex array with ID of " << vertexArrayObjectID << std::endl;
}

void VertexArrayObject::LinkAttribute(VertexBufferObject& vertexBufferObject, GLuint layout, GLuint componentCount, GLenum type, GLboolean isNormalized, GLsizeiptr stride, void* offset)
{
	vertexBufferObject.Bind();
	glVertexAttribPointer(layout, componentCount, type, isNormalized, (GLsizei)stride, offset);
	glEnableVertexAttribArray(layout);
	vertexBufferObject.Unbind();
}

void VertexArrayObject::Bind()
{
	glBindVertexArray(vertexArrayObjectID);
}

void VertexArrayObject::Unbind()
{
	glBindVertexArray(0);
}

void VertexArrayObject::Delete()
{
	glDeleteVertexArrays(1, &vertexArrayObjectID);
}