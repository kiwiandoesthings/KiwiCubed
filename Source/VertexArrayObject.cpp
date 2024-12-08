#include "VertexArrayObject.h"

VertexArrayObject::VertexArrayObject(/*const char* initializer = "(unknown)"*/) : vertexArrayObjectID(0) {
}

int VertexArrayObject::SetupArrayObject() {
	GLCall(glGenVertexArrays(1, &vertexArrayObjectID));
	//std::cout << "Vertex Array Object Setup / Info: Successfully created vertex array with ID of " << vertexArrayObjectID << std::endl;

	return vertexArrayObjectID;
}

void VertexArrayObject::LinkAttribute(VertexBufferObject& vertexBufferObject, GLuint layout, GLuint componentCount, GLenum type, GLboolean isNormalized, GLsizeiptr stride, void* offset) {
	vertexBufferObject.Bind();
	GLCall(glVertexAttribPointer(layout, componentCount, type, isNormalized, (GLsizei)stride, offset));
	GLCall(glEnableVertexAttribArray(layout));
	vertexBufferObject.Unbind();
}

void VertexArrayObject::Bind() const {
	GLCall(glBindVertexArray(vertexArrayObjectID));
}

void VertexArrayObject::Unbind() {
	GLCall(glBindVertexArray(0));
}

void VertexArrayObject::Delete() const {
	GLCall(glDeleteVertexArrays(1, &vertexArrayObjectID));
}