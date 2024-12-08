#include "Shader.h"


Shader::Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath) {
    std::string vertexSource = ParseShader(vertexFilePath);
    std::string fragmentSource = ParseShader(fragmentFilePath);

    if (!fragmentSource.empty() || !vertexSource.empty()) {
        shaderProgramID = CreateShader(vertexSource, fragmentSource);
        std::cout << "[Shader Setup / Info] Successfully created shader program with ID of " << shaderProgramID << std::endl;
    }
    else {
        std::cerr << "[Shader Setup / Error] Shader file(s) not found, aborting shader creation" << std::endl;
    }
}

std::string Shader::ParseShader(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file)
    {
        std::cerr << "[Shader Parsing / Error] Could not find shader at " << filePath << std::endl;
        return "";
    }
    std::string str;
    std::string content;
    while (std::getline(file, str)) {
        content.append(str + "\n");
    }
    return content;
}

int Shader::CompileShader(unsigned int type, const std::string& source) {
    unsigned int shaderProgramID = glCreateShader(type);
    const char* src = &source[0];
    GLCall(glShaderSource(shaderProgramID, 1, &src, nullptr));
    GLCall(glCompileShader(shaderProgramID));

    int result;
    GLCall(glGetShaderiv(shaderProgramID, GL_COMPILE_STATUS, &result));
    if (!result)
    {
        int length;
        GLCall(glGetShaderiv(shaderProgramID, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)_malloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(shaderProgramID, length, &length, message));
        std::cerr << "[Shader Compilation / Error] Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
        std::cerr << "[Shader Compilation / Error] Shader compilation error message: " << std::endl << message << std::endl;
        GLCall(glDeleteShader(shaderProgramID));
        return -1;
    }

    return shaderProgramID;
}

int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int shaderProgram = glCreateProgram();
    int vertexShaderResult = CompileShader(GL_VERTEX_SHADER, vertexShader);
    int fragmentShaderResult = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    if (vertexShaderResult == -1 || fragmentShaderResult == -1) {
        __debugbreak();
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
    unsigned int uniformLocation = glGetUniformLocation(shaderProgramID, uniform);

    if (uniformLocation == -1) {
        std::cerr << "[Shader Uniform Setting / Error] Could not find uniform: " << uniform << " in the specified shader with ID of " << shaderProgramID << std::endl;
    }
    return uniformLocation;
}

void Shader::SetUniform4fv(const char* uniform, glm::mat4 value) const {
    unsigned int uniformLocation = UniformTest(uniform);

    if (uniformLocation == -1) {
        return;
    }

    GLCall(glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value)));
}

void Shader::SetUniform1ui(const char* uniform, unsigned int value) const {
    unsigned int uniformLocation = UniformTest(uniform);

    if (uniformLocation == -1) {
        return;
    }

    GLCall(glUniform1ui(uniformLocation, value));
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