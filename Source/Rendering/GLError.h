#pragma once

#include "klogger.hpp"
#include <format>
#include <glad/glad.h>
#include <iostream>

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
