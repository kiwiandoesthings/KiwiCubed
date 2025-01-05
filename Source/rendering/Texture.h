#pragma once

#include <GLError.h>
#include <klogger.hpp>

#include"Shader.h"

class Texture {
	public:
		GLuint ID;
		GLenum type;
		GLuint atlasSize;
	
		Texture() : ID(0), type(0), atlasSize(0) {}
		Texture(const char* filepath, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);
	
		void TextureUnit(Shader& shader, const char* uniform, GLuint unit);
		void SetAtlasSize(Shader& shader, GLuint newAtlasSize);
		void Bind() const;
		void Unbind() const;
	
		void Delete() const;
};