#pragma once

#include <debug-trap.h>
#include <glad/glad.h>

#include <iostream>

#include "klogger.hpp"

#define ASSERT(x)                                                                                                                          \
    if (!(x))                                                                                                                              \
        psnip_trap();

std::string GLStringifyError(GLenum errcode);

static GLenum GLCALL_errorCode = GL_NO_ERROR;

#define GLCall(x)                                                                                                                          \
    while (glGetError() != GL_NO_ERROR) {                                                                                                  \
    };                                                                                                                                     \
    x;                                                                                                                                     \
    while ((GLCALL_errorCode = glGetError()) != GL_NO_ERROR) {                                                                             \
        ERR(std::format("GL {} code {}", #x, GLStringifyError(GLCALL_errorCode).c_str()));                                                 \
    }
