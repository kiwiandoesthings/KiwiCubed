#include "Window.h"

#include <cstring>

SDL_Window *Window::CreateWindowInstance(int windowWidth, int windowHeight, const char *windowTitle, const char *windowType) {
    OVERRIDE_LOG_NAME("Initialization");
    Window::windowWidth = windowWidth;
    Window::windowHeight = windowHeight;
    Window::windowTitle = windowTitle;

#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    glsl_version = (char *)"#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    glsl_version = (char *)"#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    glsl_version = (char *)"#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // window types: fullscreen, window_borderless, window_maximized, window
    SDL_WindowFlags flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, flags);

    if (window == nullptr) {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        exit(0);
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        exit(0);
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    return window;
}

void Window::Setup() {
    inputHandler.RegisterKeyCallback(SDL_SCANCODE_ESCAPE, [&]() { isFocused = false; });
    inputHandler.RegisterKeyCallback(SDL_SCANCODE_SPACE, [&]() {
        isFocused = true;
        SDL_WarpMouseInWindow(window, static_cast<float>(windowWidth) / 2, static_cast<float>(windowHeight) / 2);
    });
}

void Window::UpdateWindowSize(int newWindowWidth, int newWindowHeight) {
    windowWidth = newWindowWidth;
    windowHeight = newWindowHeight;
}

int Window::GetWidth() const { return windowWidth; }

int Window::GetHeight() const { return windowHeight; }

const char *Window::GetTitle() const { return windowTitle; }

void Window::SetTitle(const char *newTitle) {
    SDL_SetWindowTitle(window, newTitle);
    windowTitle = newTitle;
}

void Window::SetTitle(std::string newTitle) {
    SDL_SetWindowTitle(window, newTitle.c_str());
    windowTitle = newTitle.c_str();
}

SDL_Window *Window::GetWindowInstance() { return window; }

void Window::Delete() {
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
