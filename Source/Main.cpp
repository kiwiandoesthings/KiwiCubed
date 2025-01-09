// In loving memory of Riley.
// ~2008-2/8/2024
// R.I.P.

bool bitness;
#define SDL_MAIN_HANDLED

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <time.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <klogger.hpp>
#include <nlohmann/json.hpp>
#include <string>

#include "DataPanel.h"
#include "DebugRenderer.h"
#include "IncludeGL.h"
#include "Input.h"
#include "Renderer.h"
#include "Shader.h"
#include "SingleplayerHandler.h"
#include "Texture.h"
#include "Window.h"

// Make it so on laptops, it will request the dGPU if possible, without this,
// you have to force it to use the dGPU
#ifdef __linux__
// TEMP FIX BECAUSE LINUX SETENV NO EXISTING

// For NVIDIA GPUs
// setenv("__NV_PRIME_RENDER_OFFLOAD", "1", 1);
// setenv("__GLX_VENDOR_LIBRARY_NAME", "nvidia", 1);

// For AMD GPUs
// setenv("DRI_PRIME", "1", 1);
#elif _WIN32
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; // For NVIDIA GPUs
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;   // For AMD GPUs
}
#endif

// Signal handler (Signals don't work on windows iirc so block it from there for the time being)
#if defined(__linux__) || defined(__APPLE__)
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}
#endif

using json = nlohmann::json;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

int windowWidth;
int windowHeight;
std::string windowTitle;
std::string windowType;
std::string projectVersion;

// This main function is getting out of hand
// yes but hands are in pocket so is it still in the pocket or is it out of
// pocket? - Astra
int main() {
// Install signal handler if on linux or mac
#if defined(__linux__) || defined(__APPLE__)
    signal(SIGSEGV, handler);
    signal(SIGABRT, handler);
    signal(SIGKILL, handler);
#endif

    SDL_SetMainReady();

    std::ifstream file("init_config.json");

    OVERRIDE_LOG_NAME("Initialization");

    LOG_CHECK_RETURN(file.is_open(), "Successfully opened the JSON config file", "Could not open or find the JSON config file", 1);

    json jsonData;
    file >> jsonData;

    windowWidth = jsonData["init_settings"]["window_width"];
    windowHeight = jsonData["init_settings"]["window_height"];
    windowTitle = jsonData["init_settings"]["window_title"].get<std::string>();
    windowType = jsonData["init_settings"]["window_type"].get<std::string>();
    projectVersion = jsonData["project_version"].get<std::string>();

    // Initialize SDL2
    LOG_CHECK_RETURN((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0), "Successfully initialized SDL2", "Failed to initialize SDL2", -1);

    // Create a window
    Window globalWindow;
    globalWindow.CreateWindowInstance(windowWidth, windowHeight, std::string(windowTitle + projectVersion).c_str(), windowType.c_str());
    globalWindow.Setup();

    // glfwSetFramebufferSizeCallback(globalWindow.GetWindowInstance(), framebuffer_size_callback);

    // Initialize glad
    LOG_CHECK_RETURN(
        gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress), "Successfully initialized GLAD", "Failed to initialize GLAD", -1
    );
    if (sizeof(void *) == 8) {
        bitness = 1;
    } else if (sizeof(void *) == 4) {
        bitness = 0;
    } else {
        ERR("Could not find machine bitness");
        return -1;
    }
    INFO("Machine bitness: " + std::to_string(bitness == 1 ? 64 : 32));
    INFO("Using OpenGL version: " + std::string((char *)glGetString(GL_VERSION)));
    INFO("Using graphics device: " + std::string((char *)glGetString(GL_RENDERER)));
    INFO("Using resolution: " + std::to_string(globalWindow.GetWidth()) + " x " + std::to_string(globalWindow.GetHeight()));

    // Set things up before main game loop
    GLCall(glViewport(0, 0, globalWindow.GetWidth(), globalWindow.GetHeight()));
    GLCall(glEnable(GL_DEPTH_TEST));

    INFO("Starting main game loop...");

    // Create a singleplayer world
    SingleplayerHandler singleplayerHandler(globalWindow);
    singleplayerHandler.Setup();
    singleplayerHandler.StartSingleplayerWorld();

    DataPanel datapanel(&singleplayerHandler, &globalWindow, globalWindow.glsl_version);

    INFO("Initializing shaders...");

    // Create a debug shader
    Shader terrainShaderProgram("Mods/kiwicubed/Shaders/Terrain_Vertex.vert", "Mods/kiwicubed/Shaders/Terrain_Fragment.frag");
    Shader wireframeShaderProgram("Mods/kiwicubed/Shaders/Wireframe_Vertex.vert", "Mods/kiwicubed/Shaders/Wireframe_Fragment.frag");
    Shader chunkDebugShaderProgram("Mods/kiwicubed/Shaders/ChunkDebug_Vertex.vert", "Mods/kiwicubed/Shaders/ChunkDebug_Fragment.frag");

    INFO("Initializing textures...");

    // Create a debug texture
    Texture terrainAtlas("Mods/kiwicubed/Textures/terrain_atlas.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    terrainAtlas.SetAtlasSize(terrainShaderProgram, 3);
    terrainAtlas.SetAtlasSize(chunkDebugShaderProgram, 3);

    singleplayerHandler.singleplayerWorld.GenerateWorld();
    singleplayerHandler.singleplayerWorld.SetupRenderComponents();

    terrainAtlas.Bind();

    Renderer renderer = Renderer();
    DebugRenderer debugRenderer = DebugRenderer();

    glm::vec3 c1 = singleplayerHandler.singleplayerWorld.player.GetEntityData().physicsBoundingBox.corner1;
    glm::vec3 c2 = singleplayerHandler.singleplayerWorld.player.GetEntityData().physicsBoundingBox.corner2;
    glm::vec3 pos = singleplayerHandler.singleplayerWorld.player.GetEntityData().position;

    debugRenderer.SetupBuffers(
        c1, c2, pos, singleplayerHandler.singleplayerWorld.GetChunkDebugVisualizationVertices(),
        singleplayerHandler.singleplayerWorld.GetChunkDebugVisualizationIndices(), singleplayerHandler.singleplayerWorld.GetChunkOrigins()
    );

    int frames = 0;
    auto start_time = std::chrono::high_resolution_clock::now();
    double fps = 0.0;

    SDL_Window *sdlwin = globalWindow.GetWindowInstance();

    // Main game loop
    bool exit_loop = false;
    while (!exit_loop) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                exit_loop = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(sdlwin))
                exit_loop = true;
        }
        if (SDL_GetWindowFlags(sdlwin) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        if (duration >= 1000.0) {
            fps = static_cast<float>(frames) / (duration / 1000.0);
            frames = 0;
            start_time = end_time;
        }

        EntityData playerData = singleplayerHandler.singleplayerWorld.player.GetEntityData();
        datapanel.Draw(frames, fps, playerData);

        // Make background ~pink~
        renderer.ClearScreen(0.98f, 0.88f, 1.0f);

        // Do rendering stuff
        globalWindow.QueryInputs();
        if (singleplayerHandler.isLoadedIntoSingleplayerWorld) {
            singleplayerHandler.singleplayerWorld.Update();
            singleplayerHandler.singleplayerWorld.player.UpdateCameraMatrix(terrainShaderProgram);
            singleplayerHandler.singleplayerWorld.player.UpdateCameraMatrix(wireframeShaderProgram);
            singleplayerHandler.singleplayerWorld.player.UpdateCameraMatrix(chunkDebugShaderProgram);

            singleplayerHandler.singleplayerWorld.Render(terrainShaderProgram);

            glm::vec3 c1 = singleplayerHandler.singleplayerWorld.player.GetEntityData().physicsBoundingBox.corner1;
            glm::vec3 c2 = singleplayerHandler.singleplayerWorld.player.GetEntityData().physicsBoundingBox.corner2;
            glm::vec3 pos = singleplayerHandler.singleplayerWorld.player.GetEntityData().position;

            debugRenderer.UpdateBuffers(
                c1, c2, pos, singleplayerHandler.singleplayerWorld.GetChunkDebugVisualizationVertices(),
                singleplayerHandler.singleplayerWorld.GetChunkDebugVisualizationIndices(),
                singleplayerHandler.singleplayerWorld.GetChunkOrigins()
            );
            debugRenderer.UpdateUniforms();
            debugRenderer.RenderDebug(wireframeShaderProgram, chunkDebugShaderProgram);
        }

        datapanel.SwapFrames();
        SDL_GL_SwapWindow(sdlwin);

        ++frames;
    }

    // Clean up once the program has exited
    OVERRIDE_LOG_NAME("Cleanup");
    INFO("Cleaning up...");
    singleplayerHandler.EndSingleplayerWorld();

    datapanel.Destroy();

    globalWindow.Delete();

    return 0;
}

// void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
//     if (width == 0 || height == 0) {
//         return;
//     }
//     glViewport(0, 0, width, height);
//     Window *globalWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
//     if (globalWindow) {
//         globalWindow->UpdateWindowSize(width, height);
//     }
// }
