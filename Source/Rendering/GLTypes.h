#pragma once

#include <GLError.h>

#include <iostream>
#include <vector>

class IndexBufferObject {
  public:
    GLuint indexBufferObjectID;

    IndexBufferObject() : indexBufferObjectID(0) {}

    int SetupBuffer() {
        GLCall(glGenBuffers(1, &indexBufferObjectID));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjectID));

        return indexBufferObjectID;
    };
    void SetBufferData(GLsizeiptr size, GLuint *indicies) { GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indicies, GL_STATIC_DRAW)); }
    void Bind() const { GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjectID)); };
    void Unbind() { GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)) };
    void Delete() const { GLCall(glDeleteBuffers(1, &indexBufferObjectID)) };
};

class VertexBufferObject {
  public:
    GLuint vertexBufferObjectID;

    VertexBufferObject() : vertexBufferObjectID(0) {}

    int SetupBuffer() {
        GLCall(glGenBuffers(1, &vertexBufferObjectID));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectID));
        return vertexBufferObjectID;
    }

    void SetBufferData(GLsizeiptr size, GLfloat *vertices) { GLCall(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW)); }

    void Bind() const { GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectID)); }

    void Unbind() { GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); }

    void Delete() const { GLCall(glDeleteBuffers(1, &vertexBufferObjectID)); }
};

class VertexArrayObject {
  public:
    GLuint vertexArrayObjectID;

    VertexArrayObject() : vertexArrayObjectID(0) {}

    int SetupArrayObject() {
        GLCall(glGenVertexArrays(1, &vertexArrayObjectID));
        return vertexArrayObjectID;
    }

    void LinkAttribute(
        VertexBufferObject &vertexBufferObject, GLuint layout, GLuint componentCount, GLenum type, GLboolean isNormalized,
        GLsizeiptr stride, void *offset
    ) {
        vertexBufferObject.Bind();
        GLCall(glVertexAttribPointer(layout, componentCount, type, isNormalized, (GLsizei)stride, offset));
        GLCall(glEnableVertexAttribArray(layout));
        vertexBufferObject.Unbind();
    }

    void Bind() const { GLCall(glBindVertexArray(vertexArrayObjectID)); }

    void Unbind() { GLCall(glBindVertexArray(0)); }

    void Delete() const { GLCall(glDeleteVertexArrays(1, &vertexArrayObjectID)); }
};
