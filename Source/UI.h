#pragma once

#include <klogger.hpp>

#include <glm/vec2.hpp>

#include "Events.h"
#include "IndexBufferObject.h"
#include "Input.h"
#include "Renderer.h"
#include "Shader.h"
//#include "TextRenderer.h"
#include "Texture.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "Window.h"


class TextRenderer;
class UIScreen;
class UIElement;


class UI {
    public:
        static UI& GetInstance();

        void Setup(Shader* shaderProgram, Texture* atlas, TextRenderer* textRenderer);
        void Render();

        void AddScreen(UIScreen* screen);
        void SetCurrentScreen(UIScreen* screen);

        UIScreen* GetScreen(std::string& screenName);
        UIScreen* GetCurrentScreen();
        std::string GetCurrentScreenName();

        InputHandler& GetInputHandler();

        Shader* uiShaderProgram;
        Texture* uiAtlas;
        TextRenderer* uiTextRenderer;
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
        bool screenVisible = true;

        InputHandler inputHandler = InputHandler();
};


class UIScreen {
    public:
        const std::string screenName;
        std::vector<UIElement*> uiElements;

        UIScreen(std::string screenName);

        void Render();

        void AddUIElement(UIElement* uiElement);

    private:
};


class UIElement {
    public:
        UIElement(glm::vec2 position, glm::vec2 size, std::string eventToTrigger, std::string elementLabel) : position(position), size(size), eventToTrigger(eventToTrigger), elementLabel(elementLabel) {}
        
        glm::vec2 PixelsToNDC(glm::vec2 pixelPosition);
        float PositionToNDC(float number);

        void Render();

        bool OnClick();
        void OnHover();

        glm::vec2* GetPosition();
        glm::vec2* GetSize();

        std::string* GetEventTrigger();
        std::string* GetElementLabel();

    protected:
        glm::vec2 position = glm::vec2(0, 0);
        glm::vec2 size = glm::vec2(0, 0);
        std::string eventToTrigger = "";
        std::string elementLabel = "";

        unsigned int textureIndex;

        bool GetHovered();
};