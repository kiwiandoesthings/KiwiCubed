#include <VertexBufferObject.h>

VertexBufferObject::VertexBufferObject(/*const char* initializer = "(unknown)"*/) : vertexBufferObjectID(0){
}

int VertexBufferObject::SetupBuffer() {
	GLCall(glGenBuffers(1, &vertexBufferObjectID));
	//std::cout << "[Vertex Buffer Object Setup / Info] Successfully created vertex buffer with ID of: " << vertexBufferObjectID << " from " << std::endl;
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectID));

	return vertexBufferObjectID;
}

void VertexBufferObject::SetBufferData(GLsizeiptr size, GLfloat* vertices) {
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW));
}

void VertexBufferObject::Bind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectID));
}

void VertexBufferObject::Unbind() {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBufferObject::Delete() const {
	//std::cout << "[Vertex Buffer Object Deletion / Info] Deleting vertex buffer with ID of: " << vertexBufferObjectID << std::endl;
	GLCall(glDeleteBuffers(1, &vertexBufferObjectID));
}