#include "Renderer.h"


void Renderer::ClearScreen(float redValue, float blueValue, float greenValue) {
	// Clear them buffers so they are nice and clean for next frame
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	// Paint over the whole screen with a fun color
	GLCall(glClearColor(redValue, blueValue, greenValue, 1.0f));
}

void Renderer::DrawElements(VertexArrayObject vertexArrayObject, VertexBufferObject vertexBufferObject, IndexBufferObject indexBufferObject, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices) {
	vertexArrayObject.Bind();
	vertexBufferObject.Bind();
	vertexBufferObject.SetBufferData(vertices.size() * sizeof(GLfloat), vertices.data());
	indexBufferObject.Bind();
	indexBufferObject.SetBufferData(indices.size() * sizeof(GLuint), indices.data());
	vertexArrayObject.LinkAttribute(vertexBufferObject, 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	vertexArrayObject.LinkAttribute(vertexBufferObject, 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoordinate));
	vertexArrayObject.LinkAttribute(vertexBufferObject, 2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureIndex));
	GLCall(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0));
}
