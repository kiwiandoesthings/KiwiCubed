#include "IndexBufferObject.h"

IndexBufferObject::IndexBufferObject()
{
	GLCall(glGenBuffers(1, &indexBufferObjectID));
	//std::cout << "Index Buffer Object Setup / Info: Successfully created index buffer with ID of " << indexBufferObjectID << std::endl;
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjectID));
}

void IndexBufferObject::Setup(GLsizeiptr size, GLuint* indicies)
{
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indicies, GL_STATIC_DRAW));
}

void IndexBufferObject::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjectID));
}

void IndexBufferObject::Unbind()
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void IndexBufferObject::Delete() const
{
	GLCall(glDeleteBuffers(1, &indexBufferObjectID));
}