#include <IndexBufferObject.h>

IndexBufferObject::IndexBufferObject(GLuint* indicies, GLsizeiptr size)
{
	glGenBuffers(1, &indexBufferObjectID);
	//std::cout << "Index Buffer Object Setup / Info: Successfully created index buffer with ID of " << indexBufferObjectID << std::endl;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjectID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indicies, GL_STATIC_DRAW);
	std::cout << "Index Buffer Object Setup / Info: Successfully created index buffer with ID of " << indexBufferObjectID << std::endl;
}

void IndexBufferObject::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjectID);
}

void IndexBufferObject::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBufferObject::Delete()
{
	glDeleteBuffers(1, &indexBufferObjectID);
}