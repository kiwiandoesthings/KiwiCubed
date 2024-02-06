#include <VertexBufferObject.h>

VertexBufferObject::VertexBufferObject(GLfloat* verticies, GLsizeiptr size)
{
	glGenBuffers(1, &vertexBufferObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectID);
	glBufferData(GL_ARRAY_BUFFER, size, verticies, GL_STATIC_DRAW);
	std::cout << "Vertex Buffer Object Setup / Info: Successfully created vertex buffer with ID of " << vertexBufferObjectID << std::endl;
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