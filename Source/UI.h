#pragma once

#include "klogger.hpp"

#include <glm/vec2.hpp>

#include "IndexBufferObject.h"
#include "Input.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "Window.h"

class UIScreen;
class UIElement;


class UI {
    public:
        static UI& GetInstance();

        void Setup(Shader* shaderProgram, Texture* atlas, Window* window);
        void Render();

        void AddScreen(UIScreen* screen);
        void SetCurrentScreen(UIScreen* screen);

        Shader* uiShaderProgram;
        Texture* uiAtlas;
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

        std::vector<UIScreen*> uiScreens;
        UIScreen* currentScreen;
};


class UIScreen {
    public:
        const std::string screenName;

        UIScreen(std::string screenName);

        void Render();

        void AddUIElement(UIElement* uiElement);

    private:
        std::vector<UIElement*> uiElements;
};


class UIElement {
    public:
        UIElement(glm::vec2 position, glm::vec2 size) : position(position), size(size) {}
        
        glm::vec2 PixelsToNDC(glm::vec2 pixelPosition);
        float PositionToNDC(float number);

        void Render();

    protected:
        glm::vec2 position = glm::vec2(0, 0);
        glm::vec2 size = glm::vec2(0, 0);

        bool GetHovered();
};