#pragma once

#include <GLAD/glad.h>

#include <iostream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) \
    GLClearError(); \
    x; \
    if (!GLLogCall(#x, __FILE__, __LINE__)) __debugbreak()

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
    bool hasError = false;
    while (GLenum error = glGetError()) {
        std::cout << "[[OpenGL Function Call]] / Error: " << error << " at line " << line << " in function " << function << " in file " << file << std::endl;
        hasError = true;
    }
    return !hasError;
}