#include <VertexBufferObject.h>

VertexBufferObject::VertexBufferObject()
{
	glGenBuffers(1, &vertexBufferObjectID);
	std::cout << "Vertex Buffer Object Setup / Info: Successfully created vertex buffer with ID of " << vertexBufferObjectID << std::endl;
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectID);
}

void VertexBufferObject::Setup(GLsizeiptr size, GLfloat* vertices)
{
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VertexBufferObject::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectID);
}

void VertexBufferObject::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBufferObject::Delete()
{
	glDeleteBuffers(1, &vertexBufferObjectID);
}