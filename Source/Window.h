#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <chrono>
#include <iostream>
#include <thread>

#include "Input.h"
#include <glad/glad.h>

#include "klogger.hpp"
#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

class Window {
  public:
    InputHandler inputHandler = InputHandler();

    Window() {};

    SDL_Window *CreateWindowInstance(int windowWidth, int windowHeight, const char *windowTitle, const char *windowType);

    void Setup();

    void UpdateWindowSize(int newWindowWidth, int newWindowHeight);

    int GetWidth() const;
    int GetHeight() const;
    const char *GetTitle() const;
    void SetTitle(const char *newTitle);
    void SetTitle(std::string newTitle);

    SDL_Window *GetWindowInstance();

    void Delete();

    int windowWidth, windowHeight;
    const char *windowTitle;
    const char *glsl_version;
    SDL_GLContext gl_context;

    bool isFocused;

  private:
    SDL_Window *window;
};
