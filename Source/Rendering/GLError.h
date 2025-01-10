#pragma once

#include <glad/glad.h>

#include <iostream>

#include "klogger.hpp"

#define GLCall(x)                                                                                                                          \
    do {                                                                                                                                   \
        while (glGetError() != GL_NO_ERROR)                                                                                                \
            ;                                                                                                                              \
        x;                                                                                                                                 \
        GLenum errorCode;                                                                                                                  \
        while ((errorCode = glGetError()) != GL_NO_ERROR) {                                                                                \
            ERR(std::format("GL {} code {}", #x, errorCode));                                                                              \
        }                                                                                                                                  \
    } while (0)
