#pragma once

#include <GLError.h>
#include <GLAD/glad.h>

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

		Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath);

		void SetUniform4fv(const char* uniform, glm::mat4 value);

		void Bind();
		void Delete();

	private:
		std::string ParseShader(const std::string& filePath);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
};