#pragma once

#include <glad/glad.h>

#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>

#include "AssetManager.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"


struct Character {
    unsigned int textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};


class TextRenderer {
    public:
        TextRenderer();
        ~TextRenderer();

        void Setup(FT_Library freeType, FT_Face fontFace, AssetStringID textShader);
        void RenderText(const std::string& text, float xPosition, float yPosition, float scale, glm::vec3 textColor);

        glm::vec2 MeasureText(const std::string& text, float scale);

    private:
        FT_Library freeType;
        FT_Face fontFace;

        std::map<char, Character> characters;

        Shader* textShader;

        VertexArrayObject vertexArrayObject;
        VertexBufferObject vertexBufferObject;
        IndexBufferObject indexBufferObject;

        glm::vec2 PixelsToNDC(glm::vec2 pixelPosition);
};