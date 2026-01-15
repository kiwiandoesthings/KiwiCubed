#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <klogger.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "DebugRenderer.h"
#include "Globals.h"
#include "ModHandler.h"
#include "Renderer.h"
#include "SingleplayerHandler.h"
#include "TextRenderer.h"
#include "UI.h"
#include "Window.h"

class KiwiCubedEngine {
    public:
        KiwiCubedEngine();

        int StartEngine();
        bool RunGameLoop();
        void Exit();

    private:
        // Singletons
        EventManager& eventManager = EventManager::GetInstance();
        Globals& globals = Globals::GetInstance();
        ModHandler& modHandler = ModHandler::GetInstance();
        UI& ui = UI::GetInstance();
        Window& globalWindow = Window::GetInstance();

        // Other Members
        DebugRenderer debugRenderer = DebugRenderer();
        Renderer renderer = Renderer();
        SingleplayerHandler singleplayerHandler = SingleplayerHandler();
        TextRenderer textRenderer = TextRenderer();

        // Temporary
        GLFWwindow* glfwWindow = nullptr;
        Shader* terrainShaderProgram = nullptr;
        Shader* wireframeShaderProgram = nullptr;
        Shader* chunkDebugShaderProgram = nullptr;
        Shader* uiShaderProgram = nullptr;
        Shader* entityShaderProgram = nullptr;
        Texture* terrainAtlas = nullptr;
};