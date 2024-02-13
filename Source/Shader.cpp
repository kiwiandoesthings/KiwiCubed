#include <Shader.h>


Shader::Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath)
{
	std::string vertexSource = ParseShader(vertexFilePath);
	std::string fragmentSource = ParseShader(fragmentFilePath);

    shaderProgramID = CreateShader(vertexSource, fragmentSource);
    std::cout << "Shader Setup / Info: Successfully created shader program with ID of " << shaderProgramID << std::endl;
}

std::string Shader::ParseShader(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file)
    {
        std::cerr << "Shader Parsing / Warning: Could not find shader at " << filePath << std::endl;
    }
    std::string str;
    std::string content;
    while (std::getline(file, str)) {
        content.append(str + "\n");
    }
    return content;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
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
        std::cerr << "Shader Compilation / Error: Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
        std::cerr << "Shader Compilation / Error: Shader compilation error message, " << message << std::endl;
        GLCall(glDeleteShader(shaderProgramID));
        return 0;
    }

    return shaderProgramID;
}

unsigned int Shader::CreateShader(std::string& vertexShader, std::string& fragmentShader)
{
    unsigned int shaderProgram = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(shaderProgram, vs));
    GLCall(glAttachShader(shaderProgram, fs));
    GLCall(glLinkProgram(shaderProgram));
    GLCall(glValidateProgram(shaderProgram));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return shaderProgram;
}

void Shader::Bind()
{
    GLCall(glUseProgram(shaderProgramID));
}

void Shader::Delete()
{
    GLCall(glDeleteProgram(shaderProgramID));
}