#include "Shader.h"
#include <debug-trap.h>
#include <string>
#include <klogger.hpp>

Shader::Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath) {
    std::string vertexSource = ParseShader(vertexFilePath);
    std::string fragmentSource = ParseShader(fragmentFilePath);

    OVERRIDE_LOG_NAME("Shader Setup");

    if (!fragmentSource.empty() || !vertexSource.empty()) {
        shaderProgramID = CreateShader(vertexSource, fragmentSource, vertexFilePath, fragmentFilePath);

        if (vertexFilePath.find_last_of('/') != std::string::npos) {
            shaderName = vertexFilePath.substr(vertexFilePath.find_last_of('/') + 1);
        }
        
        if (shaderName.find('_') != std::string::npos) {
            shaderName = shaderName.substr(0, shaderName.find('_'));
        }

        INFO("Successfully created " + shaderName + " shader program with ID of " + std::to_string(shaderProgramID));
    }
    else {
        ERR("Shader file(s) not found, aborting shader creation");
    }
}

std::string Shader::ParseShader(const std::string& filePath) {
    OVERRIDE_LOG_NAME("Shader File Parsing");
    std::ifstream file(filePath);
    LOG_CHECK_RETURN(file.is_open(), "Successfully opened shader file at " + filePath, "Could not open shader file at " + filePath, "");
    std::string str;
    std::string content;
    while (std::getline(file, str)) {
        content.append(str + "\n");
    }
    return content;
}

int Shader::CompileShader(unsigned int type, const std::string& source, const std::string& filePath) {
    OVERRIDE_LOG_NAME("Shader Compilation");
    unsigned int shaderProgramID = glCreateShader(type);
    const char* src = &source[0];
    GLCall(glShaderSource(shaderProgramID, 1, &src, nullptr));
    GLCall(glCompileShader(shaderProgramID));

    int result;
    GLCall(glGetShaderiv(shaderProgramID, GL_COMPILE_STATUS, &result));
    if (!result) {
        int length;
        GLCall(glGetShaderiv(shaderProgramID, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)malloc(length * sizeof(char));
        GLCall(glGetShaderInfoLog(shaderProgramID, length, &length, message));
        ERR("Failed to compile " + std::string((type == GL_VERTEX_SHADER ? "vertex" : "fragment")) + " shader from file " + filePath);
        ERR("Shader compilation error message:  \n" + std::string(message));
        GLCall(glDeleteShader(shaderProgramID));
        return -1;
    }

    return shaderProgramID;
}

int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& vertexFilePath, const std::string& fragmentFilePath) {
    unsigned int shaderProgram = glCreateProgram();
    int vertexShaderResult = CompileShader(GL_VERTEX_SHADER, vertexShader, vertexFilePath);
    int fragmentShaderResult = CompileShader(GL_FRAGMENT_SHADER, fragmentShader, fragmentFilePath);
    if (vertexShaderResult == -1 || fragmentShaderResult == -1) {
        psnip_trap();
    }

    GLCall(glAttachShader(shaderProgram, vertexShaderResult));
    GLCall(glAttachShader(shaderProgram, fragmentShaderResult));
    GLCall(glLinkProgram(shaderProgram));
    GLCall(glValidateProgram(shaderProgram));

    GLCall(glDeleteShader(vertexShaderResult));
    GLCall(glDeleteShader(fragmentShaderResult));

    return shaderProgram;
}

unsigned int Shader::UniformTest(const char* uniform) const {
    OVERRIDE_LOG_NAME("Shader Uniform Setting");
    unsigned int uniformLocation = glGetUniformLocation(shaderProgramID, uniform);

    if (uniformLocation == -1) {
        WARN("Could not find uniform: " + std::string(uniform) + " in " + shaderName + " shader with ID of " + std::to_string(shaderProgramID));
    }
    return uniformLocation;
}

void Shader::SetUniform1ui(const char* uniform, unsigned int value) const {
    Bind();
    unsigned int uniformLocation = UniformTest(uniform);

    if (uniformLocation == -1) {
        return;
    }

    GLCall(glUniform1ui(uniformLocation, value));
}

void Shader::SetUniform3fv(const char* uniform, glm::vec3 value) const {
    Bind();
    unsigned int uniformLocation = UniformTest(uniform);

    if (uniformLocation == -1) {
        return;
    }

    GLCall(glUniform3fv(uniformLocation, 1, glm::value_ptr(value)));
}

void Shader::SetUniform4fv(const char* uniform, glm::mat4 value) const {
    Bind();
    unsigned int uniformLocation = UniformTest(uniform);

    if (uniformLocation == -1) {
        return;
    }

    GLCall(glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value)));
}

void Shader::Bind() const {
    GLCall(glUseProgram(shaderProgramID));
}

void Shader::Unbind() const {
    GLCall(glUseProgram(0));
}

void Shader::Delete() const {
    GLCall(glDeleteProgram(shaderProgramID));
}