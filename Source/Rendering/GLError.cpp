#include "GLError.h"

std::string GLStringifyError(GLenum errcode) {
    switch (errcode) {
    case GL_INVALID_ENUM:
        return "INVALID_ENUM";
        break;
    case GL_INVALID_VALUE:
        return "INVALID_VALUE";
        break;
    case GL_INVALID_OPERATION:
        return "INVALID_OPERATION";
        break;
    case GL_STACK_OVERFLOW:
        return "STACK_OVERFLOW";
        break;
    case GL_STACK_UNDERFLOW:
        return "STACK_UNDERFLOW";
        break;
    case GL_OUT_OF_MEMORY:
        return "OUT_OF_MEMORY";
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "INVALID_FRAMEBUFFER_OPERATION";
        break;
    default:
        return "UNKNOWN_ERROR_CODE_" + std::to_string(errcode);
        break;
    }
}
