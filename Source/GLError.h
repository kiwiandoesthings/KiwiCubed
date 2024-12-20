#pragma once

#include <glad/glad.h>
#include <iostream>
#include <debug-trap.h>


#define ASSERT(x) if (!(x)) psnip_trap();
#define GLCall(x) \
    GLClearError(); \
    x; \
    if (!GLLogCall(#x, __FILE__, __LINE__)) psnip_trap()

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
    bool hasError = false;
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << "[[OpenGL Function Call]] / Error: " << error << " / " << errorCode << " at line " << line << " in function " << function << " in file " << file << std::endl;
        hasError = true;
    }
    return !hasError;
}