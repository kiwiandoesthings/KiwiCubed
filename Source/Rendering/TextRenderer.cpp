#include "TextRenderer.h"
#include "freetype/freetype.h"

TextRenderer::TextRenderer(FT_Library& freeType, FT_Face& fontFace, Shader& textShader) : freeType(freeType), fontFace(fontFace), textShader(textShader) {
    vertexArrayObject.SetupArrayObject();
    vertexBufferObject.SetupBuffer();
    indexBufferObject.SetupBuffer();

    vertexArrayObject.Bind();
    vertexBufferObject.Bind();
    vertexBufferObject.SetBufferData(sizeof(float) * 6 * 4, NULL);
    vertexArrayObject.LinkAttribute(vertexBufferObject, 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	for (unsigned char iterator = 0; iterator < 128; ++iterator) {
		if (FT_Load_Char(fontFace, iterator, FT_LOAD_RENDER)) {
			std::cerr << "Failed to load glyph " << iterator << std::endl;
			continue;
		}

		unsigned int texture;
        GLCall(glActiveTexture(GL_TEXTURE3));
		GLCall(glGenTextures(1, &texture));
		GLCall(glBindTexture(GL_TEXTURE_2D, texture));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, fontFace->glyph->bitmap.buffer));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    	Character character = {
    	    texture,
    	    glm::ivec2(fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows),
    	    glm::ivec2(fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top),
    	    static_cast<unsigned int>(fontFace->glyph->advance.x)
    	};

    	characters.insert(std::pair<char, Character>(iterator, character));
	}
}

TextRenderer::~TextRenderer() {
    characters.clear();

    FT_Done_Face(fontFace);
	FT_Done_FreeType(freeType);
}

void TextRenderer::RenderText(const std::string& text, float xPosition, float yPosition, float scale, glm::vec3 textColor) {
    textShader.Bind();
    textShader.SetUniform3fv("textColor", textColor /  glm::vec3(255));
    textShader.SetUniformMatrix4fv("modelMatrix", glm::mat4(1.0f));

    GLCall(glActiveTexture(GL_TEXTURE0));
    vertexArrayObject.Bind();

    float temporaryY = yPosition;
    yPosition = xPosition;
    xPosition = temporaryY;

    for (char iterator : text) {
        Character character = characters[iterator];

        glm::vec2 characterPosition = PixelsToNDC(glm::vec2(xPosition + character.bearing.x * scale, yPosition - (character.size.y - character.bearing.y) * scale));
        glm::vec2 characterSize = PixelsToNDC(glm::vec2(character.size.x * scale, character.size.y * scale)) + glm::vec2(1, 1);

        float vertices[6][4] = {
            { characterPosition.x,     characterPosition.y + characterSize.y,   0.0f, 0.0f },            
            { characterPosition.x,     characterPosition.y,       0.0f, 1.0f },
            { characterPosition.x + characterSize.x, characterPosition.y,       1.0f, 1.0f },

            { characterPosition.x,     characterPosition.y + characterSize.y,   0.0f, 0.0f },
            { characterPosition.x + characterSize.x, characterPosition.y,       1.0f, 1.0f },
            { characterPosition.x + characterSize.x, characterPosition.y + characterSize.y,   1.0f, 0.0f }           
        };

        GLCall(glBindTexture(GL_TEXTURE_2D, character.textureID));

        vertexBufferObject.Bind();
        vertexBufferObject.SetBufferSubData(0, sizeof(vertices), *vertices);
        vertexArrayObject.Bind();

        GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

        xPosition += (character.advance >> 6) * scale;
    }

    vertexArrayObject.Unbind();
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

glm::vec2 TextRenderer::PixelsToNDC(glm::vec2 pixelPosition) {
    return glm::vec2((pixelPosition.x / Window::GetInstance().GetWidth()) * 2 - 1, (pixelPosition.y / Window::GetInstance().GetHeight()) * 2 - 1);
}