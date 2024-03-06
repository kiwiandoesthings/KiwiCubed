#include <Texture.h>

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	type = texType;

	int widthImg, heightImg, numColCh;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);


	GLCall(glGenTextures(1, &ID));
	GLCall(glActiveTexture(slot));
	GLCall(glBindTexture(texType, ID));

	GLCall(glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
	GLCall(glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST));


	GLCall(glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT));

	GLCall(glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes));
	GLCall(glGenerateMipmap(texType));

	stbi_image_free(bytes);
	GLCall(glBindTexture(texType, 0));
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	GLuint texUni = glGetUniformLocation(shader.shaderProgramID, uniform);
	shader.Bind();
	GLCall(glUniform1i(texUni, unit));
}

void Texture::Bind()
{
	GLCall(glBindTexture(type, ID));
}

void Texture::Unbind()
{
	GLCall(glBindTexture(type, 0));
}

void Texture::Delete()
{
	GLCall(glDeleteTextures(1, &ID));
}