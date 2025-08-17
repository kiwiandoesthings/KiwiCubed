#pragma once

#include "klogger.hpp"

#include <glm/vec2.hpp>

#include "IndexBufferObject.h"
#include "Input.h"
#include "Renderer.h"
#include "Shader.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "Window.h"


class UI {
    public:
        static UI& GetInstance();

        void Setup(Shader* shaderProgram, Window* window);

        Shader* uiShaderProgram;
        Window* window;

        VertexBufferObject vertexBufferObject;
        VertexArrayObject vertexArrayObject;
        IndexBufferObject indexBufferObject;

    private:
        UI();
        ~UI();

        UI(const UI&) = delete;
        UI& operator=(const UI&) = delete;

        bool renderComponentsSetup = false;
};


class UIElement {
    public:
        UIElement(glm::vec2 position, glm::vec2 size) : position(position), size(size) {}
        
        glm::vec2 PixelsToNDC(glm::vec2 pixelPosition);

        void Render();


    protected:
        glm::vec2 position = glm::vec2(0, 0);
        glm::vec2 size = glm::vec2(0, 0);


        bool GetHovered();
};