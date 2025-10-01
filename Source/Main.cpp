// In loving memory of Riley.
// ~2008-2/8/2024
// R.I.P.

#ifdef _WIN32
	extern "C"
	{
		__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; // For NVIDIA GPUs
		__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;   // For AMD GPUs
	}
#endif

bool bitness;


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <klogger.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <nlohmann/json.hpp>

#include "Entity.h"
#include "Events.h"
#include "DebugRenderer.h"
#include "Globals.h"
#include "Input.h"
#include "ModHandler.h"
#include "Renderer.h"
#include "Shader.h"
#include "SingleplayerHandler.h"
#include "TextRenderer.h"
#include "Texture.h"
#include "UI.h"
#include "Window.h"


using json = nlohmann::json;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);


std::string projectVersion;
int windowWidth;
int windowHeight;
std::string windowTitle;
std::string windowType;
bool debugMode;

// This main function is getting out of hand
// yes but hands are in pocket so is it still in the pocket or is it out of pocket? - Astra
int main() {
	OVERRIDE_LOG_NAME("Initialization");
	// Make it so on laptops, it will request the dGPU if possible, without this, you have to force it to use the dGPU
	#ifdef __linux__
	// For NVIDIA GPUs
	setenv("__NV_PRIME_RENDER_OFFLOAD", "1", 1);
	setenv("__GLX_VENDOR_LIBRARY_NAME", "nvidia", 1);

	// For AMD GPUs
	setenv("DRI_PRIME", "1", 1);
	#endif

	std::ifstream file("init_config.json");


	LOG_CHECK_RETURN_CRITICAL(file.is_open(), "Successfully opened the JSON config file", "Failed to open or find the JSON config file, exiting", -1);

	json jsonData;
	file >> jsonData;
	
	projectVersion = jsonData["project_version"].get<std::string>();
	windowWidth = jsonData["init_settings"]["window_width"];
	windowHeight = jsonData["init_settings"]["window_height"];
	windowTitle = jsonData["init_settings"]["window_title"].get<std::string>();
	windowType = jsonData["init_settings"]["window_type"].get<std::string>();
	debugMode = jsonData["init_settings"]["debug_mode"];

	Globals::GetInstance().debugMode = debugMode;

	// Initialize GLFW
	LOG_CHECK_RETURN_CRITICAL(glfwInit(), "Successfully initialized GLFW", "Failed to initialize GLFW, exiting", -1);

	// Create a window
	Window& globalWindow = Window::GetInstance();
	globalWindow.CreateWindowInstance(windowWidth, windowHeight, std::string(windowTitle + projectVersion).c_str(), windowType.c_str());

	glfwSetWindowUserPointer(globalWindow.GetWindowInstance(), &globalWindow);
	glfwSetFramebufferSizeCallback(globalWindow.GetWindowInstance(), framebuffer_size_callback);

	// Initialize glad
	LOG_CHECK_RETURN_CRITICAL(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Successfully initialized GLAD", "Failed to initialize GLAD, exiting", -1);

	// Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	io.IniFilename = NULL; // don't save the accursed imgui.ini
	ImGui_ImplGlfw_InitForOpenGL(globalWindow.GetWindowInstance(), true);
	ImGui_ImplOpenGL3_Init("#version 430");

	// Setup window *after* ImGui is initialized
	globalWindow.Setup();

	// System info
	if (sizeof(void*) == 8) {
		bitness = 1;
	} else if (sizeof(void*) == 4) {
		bitness = 0;
	} else {
		ERR("Failed to find machine bitness");
		return -1;
	}
	INFO("Machine bitness: " + std::to_string(bitness == 1 ? 64 : 32));
	INFO("Using OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));
	INFO("Using graphics device: " + std::string((char*)glGetString(GL_RENDERER)));
	INFO("Using resolution: " + std::to_string(globalWindow.GetWidth()) + " x " + std::to_string(globalWindow.GetHeight()));

	// Set things up before main game loop
	GLCall(glViewport(0, 0, globalWindow.GetWidth(), globalWindow.GetHeight()));
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	// Initialize FreeType
	FT_Library ft;
    LOG_CHECK_RETURN_CRITICAL(!FT_Init_FreeType(&ft), "Successfully initialized FreeType", "Failed to initialize FreeType, exiting", -1);

    FT_Face face;
    LOG_CHECK_RETURN_CRITICAL(!FT_New_Face(ft, "Mods/kiwicubed/Resources/Fonts/PixiFont.ttf", 0, &face), "Successfully loaded font PixiFont.ttf", "Failed to load font PixiFont.ttf, exiting", -1);

    FT_Set_Pixel_Sizes(face, 0, 32);

	Shader textShader("Mods/kiwicubed/Resources/Shaders/Text_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/Text_Fragment.frag");

	TextRenderer textRenderer(ft, face, textShader);

	// MAIN PROGRAM SETUP FINISHED - Most of the rest of this is able to be moved into other places to make this more modular
	
	// Create shader programs (needs to be modularized)
	Shader terrainShaderProgram("Mods/kiwicubed/Resources/Shaders/Terrain_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/Terrain_Fragment.frag");
	Shader wireframeShaderProgram("Mods/kiwicubed/Resources/Shaders/Wireframe_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/Wireframe_Fragment.frag");
	Shader chunkDebugShaderProgram("Mods/kiwicubed/Resources/Shaders/ChunkDebug_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/ChunkDebug_Fragment.frag");
	Shader uiShaderProgram("Mods/kiwicubed/Resources/Shaders/UI_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/UI_Fragment.frag");

	// Create texture atlases (needs to be modularized)
	Texture terrainAtlas("Mods/kiwicubed/Resources/Textures/terrain_atlas.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE, "texture/terrain");
	terrainAtlas.SetAtlasSize(terrainShaderProgram, glm::vec2(4, 4));
	terrainAtlas.SetAtlasSize(chunkDebugShaderProgram, glm::vec2(4, 4));
	terrainAtlas.TextureUnit(terrainShaderProgram, "tex0");
	terrainAtlas.TextureUnit(chunkDebugShaderProgram, "tex0");
	Texture uiAtlas("Mods/kiwicubed/Resources/Textures/ui_atlas.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE, "texture/gui");
	uiAtlas.SetAtlasSize(uiShaderProgram, glm::vec2(3, 1));
	uiAtlas.TextureUnit(uiShaderProgram, "tex0");

	ModHandler modHandler = ModHandler();
	LOG_CHECK_RETURN_BAD_CRITICAL(modHandler.SetupTextureAtlasData(), "Failed to setup texture atlas data, exiting", -1);

	// Setup debug renderer
	Renderer renderer = Renderer();
	DebugRenderer debugRenderer = DebugRenderer();

	UI& ui = UI::GetInstance();
	ui.Setup(&uiShaderProgram, &uiAtlas, &textRenderer);
	UIScreen mainMenuUI = UIScreen("ui/main_menu");
	mainMenuUI.AddUIElement(new UIElement(glm::vec2((globalWindow.GetWidth() / 2) - 256, 700), glm::vec2(1, 1), "event/generate_world", "Create World"));
	mainMenuUI.AddUIElement(new UIElement(glm::vec2((globalWindow.GetWidth() / 2) - 256, 500), glm::vec2(1, 1), "ui/move_screen_settings", "Settings"));
	ui.AddScreen(&mainMenuUI);
	ui.SetCurrentScreen(&mainMenuUI);

	UIScreen settingsUI = UIScreen("ui/settings");
	settingsUI.AddUIElement(new UIElement(glm::vec2((globalWindow.GetWidth() / 2) - 256, 500), glm::vec2(1, 1), "event/back_ui", "Back"));
	ui.AddScreen(&settingsUI);

	UIScreen gamePauseUI = UIScreen("ui/game_pause");
	gamePauseUI.AddUIElement(new UIElement(glm::vec2((globalWindow.GetWidth() / 2) - 256, 700), glm::vec2(1, 1), "event/disable_ui", "Resume game"));
	gamePauseUI.AddUIElement(new UIElement(glm::vec2((globalWindow.GetWidth() / 2) - 256, 500), glm::vec2(1, 1), "ui/move_screen_settings", "Settings"));
	gamePauseUI.AddUIElement(new UIElement(glm::vec2((globalWindow.GetWidth() / 2) - 256, 300), glm::vec2(1, 1), "event/unload_world", "Quit Game"));
	ui.AddScreen(&gamePauseUI);

	// Create a singleplayer instance
	SingleplayerHandler singleplayerHandler = SingleplayerHandler(debugRenderer);
	
	// FPS code
	int frames = 0;
	auto startTime = std::chrono::high_resolution_clock::now();
	double fps = 0.0;

	// Main game loop
	while (!glfwWindowShouldClose(globalWindow.GetWindowInstance())) {
		auto frameStartTime = std::chrono::high_resolution_clock::now();
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		Globals& globals = Globals::GetInstance();

		auto endTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

		if (duration >= 1000.0) {
			fps = static_cast<float>(frames) / (duration / 1000.0);
			frames = 0;
			startTime = endTime;
		}

		ImGui::Begin("Debug");
		ImGui::Text("Total frames: %d", frames);
		ImGui::Text("FPS: %.2f", fps);
		ImGui::Text("DeltaTime: %.6f", globals.deltaTime);

		if (singleplayerHandler.isLoadedIntoSingleplayerWorld) {
			if (ImGui::CollapsingHeader("Player")) {
				singleplayerHandler.singleplayerWorld->DisplayImGui(0);
			}

			if (ImGui::CollapsingHeader("World")) {
				singleplayerHandler.singleplayerWorld->DisplayImGui(1);
			}

			if (ImGui::CollapsingHeader("Chunk Info")) {
				singleplayerHandler.singleplayerWorld->DisplayImGui(2);
			}
		}

		// Make background ~pink~
		renderer.ClearScreen(0.98f, 0.88f, 1.0f);
		// Do rendering stuff
		globalWindow.QueryInputs();
		if (singleplayerHandler.isLoadedIntoSingleplayerWorld) {
			singleplayerHandler.Update();
		}

		if (singleplayerHandler.isLoadedIntoSingleplayerWorld) {
			singleplayerHandler.singleplayerWorld->GetPlayer().Update();
			singleplayerHandler.singleplayerWorld->GetPlayer().UpdateCameraMatrix(terrainShaderProgram);
			singleplayerHandler.singleplayerWorld->GetPlayer().UpdateCameraMatrix(wireframeShaderProgram);
			singleplayerHandler.singleplayerWorld->GetPlayer().UpdateCameraMatrix(chunkDebugShaderProgram);
			
			terrainAtlas.SetActive();
			terrainAtlas.Bind();
			singleplayerHandler.singleplayerWorld->Render(terrainShaderProgram);
			
			debugRenderer.UpdateBuffers(singleplayerHandler.singleplayerWorld->GetChunkDebugVisualizationVertices(), singleplayerHandler.singleplayerWorld->GetChunkDebugVisualizationIndices(), singleplayerHandler.singleplayerWorld->GetChunkOrigins());
			debugRenderer.UpdateUniforms();
			debugRenderer.RenderDebug(chunkDebugShaderProgram);
		}
		
		UI::GetInstance().Render();

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(globalWindow.GetWindowInstance());
		++frames;

		auto frameEndTime = std::chrono::high_resolution_clock::now();
		globals.deltaTime = abs(duration_cast<std::chrono::duration<float>>(frameStartTime - frameEndTime).count());
	}


	// Clean up once the program has exited
	OVERRIDE_LOG_NAME("Cleanup");
	INFO("Cleaning up...");
	if (singleplayerHandler.isLoadedIntoSingleplayerWorld) {
		singleplayerHandler.EndSingleplayerWorld();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(globalWindow.GetWindowInstance());
	glfwTerminate();

	INFO("Finished cleaning up");
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	if (width == 0 || height == 0) {
		return;
	}
	glViewport(0, 0, width, height);
	Window* globalWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (globalWindow) {
		globalWindow->UpdateWindowSize(width, height);
	}
}