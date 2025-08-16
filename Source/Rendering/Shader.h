#pragma once

#include <GLError.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>


class Shader
{
	public:
		GLuint shaderProgramID;
		std::string shaderName = "";

		Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath);

		unsigned int UniformTest(const char* uniform) const;
		void SetUniform1ui(const char* uniform, unsigned int value) const;
		void SetUniform3fv(const char* uniform, glm::vec3 value) const;
		void SetUniform4fv(const char* uniform, glm::mat4 value) const;

		void Bind() const;
		void Unbind() const;
		void Delete() const;

	private:
		std::string ParseShader(const std::string& filePath);
		int CompileShader(unsigned int type, const std::string& source, const std::string& filePath);
		int CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& vertexFilePath, const std::string& fragmentFilePath);
};