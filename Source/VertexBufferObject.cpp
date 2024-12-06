#include <VertexBufferObject.h>

VertexBufferObject::VertexBufferObject(/*const char* initializer = "(unknown)"*/)
{
	GLCall(glGenBuffers(1, &vertexBufferObjectID));
	//std::cout << "[Vertex Buffer Object Setup / Info] Successfully created vertex buffer with ID of: " << vertexBufferObjectID << " from " << initializer << std::endl;
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectID));
}

void VertexBufferObject::Setup(GLsizeiptr size, GLfloat* vertices)
{
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW));
}

void VertexBufferObject::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectID));
}

void VertexBufferObject::Unbind()
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBufferObject::Delete() const
{
	//std::cout << "[Vertex Buffer Object Deletion / Info] Deleting vertex buffer with ID of: " << vertexBufferObjectID << std::endl;
	GLCall(glDeleteBuffers(1, &vertexBufferObjectID));
}