#include <Texture.h>

Texture::Texture(const char* filepath, GLenum textureType, GLenum slot, GLenum format, GLenum pixelType) : atlasSize(0)
{
	type = textureType;

	int imageWidth, imageHeight, colorChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(filepath, &imageWidth, &imageHeight, &colorChannels, 0);

	if (!bytes) {
		std::cout << "[Texture Loading / Error] Failed to load image from file path: " << filepath << std::endl;
		__debugbreak();
	}


	GLCall(glGenTextures(1, &ID));
	GLCall(glActiveTexture(slot));
	GLCall(glBindTexture(textureType, ID));

	GLCall(glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
	GLCall(glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST));


	GLCall(glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_REPEAT));

	GLCall(glTexImage2D(textureType, 0, GL_RGBA, imageWidth, imageHeight, 0, format, pixelType, bytes));
	GLCall(glGenerateMipmap(textureType));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.5f);

	stbi_image_free(bytes);
	GLCall(glBindTexture(textureType, 0));
}

void Texture::TextureUnit(Shader& shader, const char* uniform, GLuint unit)
{
	GLuint textureUnit = glGetUniformLocation(shader.shaderProgramID, uniform);
	shader.Bind();
	GLCall(glUniform1i(textureUnit, unit));
}

void Texture::SetAtlasSize(Shader& shader, GLuint newAtlasSize) {
	atlasSize = newAtlasSize;
	shader.Bind();
	shader.SetUniform1ui("atlasSize", atlasSize);
}

void Texture::Bind() const
{
	GLCall(glBindTexture(type, ID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(type, 0));
}

void Texture::Delete() const
{
	GLCall(glDeleteTextures(1, &ID));
}