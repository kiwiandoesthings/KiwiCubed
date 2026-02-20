#pragma once

#include <klogger.hpp>

#include <glm/vec2.hpp>
#include <stack>

#include "AssetManager.h"
#include "IndexBufferObject.h"
#include "Input.h"
#include "Renderer.h"
#include "Shader.h"
#include "TextRenderer.h"
#include "Texture.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "Window.h"


class UIScreen;
class UIElement;


class UI {
    public:
        static UI& GetInstance();
        void Delete();

        void Setup(Shader shaderProgram, Texture atlas, TextRenderer* textRenderer);
        void Render();

        void AddScreen(const std::string& screenName);
        void SetCurrentScreen(const std::string& screenName);
        void MoveScreenBack();

        UIScreen* GetScreen(const std::string& screenName);
        UIScreen* GetCurrentScreen();
        std::string GetCurrentScreenName();

        void DisableUI();
        bool IsDisabled();

        InputHandler& GetInputHandler();

        Shader uiShaderProgram;
        Texture uiAtlas;
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

        std::vector<std::unique_ptr<UIScreen>> uiScreens;
        UIScreen* currentScreen;
        bool screenVisible = true;

        std::stack<UIScreen*> stackedScreens;

        InputHandler inputHandler = InputHandler("UI");
};


class UIScreen {
    public:
        const std::string screenName;
        std::vector<UIElement*> uiElements;

        UIScreen(std::string screenName);
        void Delete();

        void Render();
        void AddCustomRenderCommand(std::function<void()> customCommand);
        void ClearCustomRenderCommands();

        void AddUIElement(UIElement* uiElement);

        int GetTabIndex();
        void SetTabIndex(int newTabIndex);

    private:
        int tabIndex = -1;

        std::vector<std::function<void()>> customRenderCommands;
};


class UIElement {
    public:
        UIElement(glm::vec2 position, glm::vec2 scale, std::string functionToTrigger);
        virtual ~UIElement();
        
        static glm::vec2 PixelsToNDC(glm::vec2 pixelPosition);

        virtual void Render();

        void Trigger();

        virtual bool OnClick();
        virtual void OnHover();

        bool GetVisible();
        void SetVisible(bool visible);

        glm::vec2* GetPosition();
        glm::ivec2* GetScale();
        glm::vec2* GetSize();

        bool GetSelected();
        virtual void SetSelected(bool selected);

    protected:
        bool visible = true;
        glm::vec2 position = glm::vec2(0, 0);
        glm::ivec2 scale = glm::ivec2(1, 1);
        glm::vec2 size = glm::vec2(512, 128);
        std::string functionToTrigger;
        bool tabSelected = false;
        bool hoverSelected = false;

        virtual bool GetHovered();
};


class UIButton : public UIElement {
    public:
        UIButton(glm::vec2 position, glm::vec2 size, std::string functionToTrigger, std::string elementLabel);
        
        void Render() override;

        bool OnClick() override;
        void OnHover() override;

        void SetSelected(bool selected) override;
        
        std::string* GetElementLabel();

    private:
        glm::vec2 size;
        std::string elementLabel = "";
        MetaTexture image;
        float frame = 0;
};


class UIImage : public UIElement {
    public:
        UIImage(glm::vec2 position, glm::vec2 size, std::string functionToTrigger, AssetStringID imageStringID, AssetStringID textureAtlasStringID);

        static void Render(glm::vec2 position, glm::vec2 size, TextureAtlasData atlasData, Texture* atlas);
        void Render() override;

    private:
        glm::vec2 size;
        MetaTexture image;
        float frame = 0;
        AssetStringID textureAtlasStringID;
};