#pragma once

#include <klogger.hpp>
#include <GLError.h>
#include <debug-trap.h>

#include"Shader.h"


class Texture {
	public:
		GLuint ID;
		GLenum type;
		glm::vec2 atlasSize;
		GLenum slot;
	
		Texture() : ID(0), type(0), atlasSize(0) {}
		Texture(const char* filepath, GLenum texType, GLenum slot, GLenum format, GLenum pixelType, std::string usage);
	
		void TextureUnit(Shader& shader, const char* uniform);
		void SetAtlasSize(Shader& shader, glm::vec2 newAtlasSize);
		void Bind() const;
		void Unbind() const;
		void SetActive() const;
	
		void Delete() const;
};