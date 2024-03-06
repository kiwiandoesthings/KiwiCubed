#pragma once

#include <GLError.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>


class Shader
{
	public: 
		GLuint shaderProgramID;

		Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath);

		void Bind();
		void Delete();

	private:
		std::string ParseShader(const std::string& filePath);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(std::string& vertexShader, std::string& fragmentShader);
};