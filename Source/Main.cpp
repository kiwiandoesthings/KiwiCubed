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
#include "Input.h"
#include "Renderer.h"
#include "Shader.h"
#include "SingleplayerHandler.h"
#include "TextRenderer.h"
#include "Texture.h"
#include "UI.h"
#include "Window.h"


using json = nlohmann::json;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);


int windowWidth;
int windowHeight;
std::string windowTitle;
std::string windowType;
std::string projectVersion;

// This main function is getting out of hand
// yes but hands are in pocket so is it still in the pocket or is it out of pocket? - Astra
int main() {
	// Make it so on laptops, it will request the dGPU if possible, without this, you have to force it to use the dGPU
	#ifdef __linux__
	// For NVIDIA GPUs
	setenv("__NV_PRIME_RENDER_OFFLOAD", "1", 1);
	setenv("__GLX_VENDOR_LIBRARY_NAME", "nvidia", 1);

	// For AMD GPUs
	setenv("DRI_PRIME", "1", 1);
	#endif

	std::ifstream file("init_config.json");

	OVERRIDE_LOG_NAME("Initialization");

	LOG_CHECK_RETURN(file.is_open(), "Successfully opened the JSON config file", "Failed to open or find the JSON config file", 1);

	json jsonData;
	file >> jsonData;

	windowWidth = jsonData["init_settings"]["window_width"];
	windowHeight = jsonData["init_settings"]["window_height"];
	windowTitle = jsonData["init_settings"]["window_title"].get<std::string>();
	windowType = jsonData["init_settings"]["window_type"].get<std::string>();
	projectVersion = jsonData["project_version"].get<std::string>();

	// Initialize GLFW
	LOG_CHECK_RETURN(glfwInit(), "Successfully initialized GLFW", "Failed to initialize GLFW", -1);

	// Create a window
	Window& globalWindow = Window::GetInstance();
	globalWindow.CreateWindowInstance(windowWidth, windowHeight, std::string(windowTitle + projectVersion).c_str(), windowType.c_str());

	glfwSetWindowUserPointer(globalWindow.GetWindowInstance(), &globalWindow);
	glfwSetFramebufferSizeCallback(globalWindow.GetWindowInstance(), framebuffer_size_callback);

	// Initialize glad
	LOG_CHECK_RETURN(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Successfully initialized GLAD", "Failed to initialize GLAD", -1);

	// Initialize FreeType
	FT_Library ft;
    LOG_CHECK_RETURN(!FT_Init_FreeType(&ft), "Successfully initialized FreeType", "Failed to initialize FreeType", 1);

    FT_Face face;
    LOG_CHECK_RETURN(!FT_New_Face(ft, "Mods/kiwicubed/Resources/Fonts/PixiFont.ttf", 0, &face), "Successfully loaded font PixiFont.ttf", "Failed to load font PixiFont.ttf", 1);

    FT_Set_Pixel_Sizes(face, 0, 64);

	Shader textShader("Mods/kiwicubed/Resources/Shaders/Text_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/Text_Fragment.frag");

	TextRenderer textRenderer(ft, face, textShader);

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

	// MAIN PROGRAM SETUP FINISHED - Most of the rest of this is able to be moved into other places to make this more modular
	
	// Create shader programs (needs to be modularized)
	Shader terrainShaderProgram("Mods/kiwicubed/Resources/Shaders/Terrain_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/Terrain_Fragment.frag");
	Shader wireframeShaderProgram("Mods/kiwicubed/Resources/Shaders/Wireframe_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/Wireframe_Fragment.frag");
	Shader chunkDebugShaderProgram("Mods/kiwicubed/Resources/Shaders/ChunkDebug_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/ChunkDebug_Fragment.frag");
	Shader uiShaderProgram("Mods/kiwicubed/Resources/Shaders/UI_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/UI_Fragment.frag");

	// Create texture atlases (needs to be modularized)
	Texture terrainAtlas("Mods/kiwicubed/Resources/Textures/terrain_atlas.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE, "texture/terrain");
	terrainAtlas.SetAtlasSize(terrainShaderProgram, glm::vec2(3, 3));
	terrainAtlas.SetAtlasSize(chunkDebugShaderProgram, glm::vec2(3, 3));
	terrainAtlas.TextureUnit(terrainShaderProgram, "tex0");
	terrainAtlas.TextureUnit(chunkDebugShaderProgram, "tex0");
	Texture uiAtlas("Mods/kiwicubed/Resources/Textures/ui_atlas.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE, "texture/gui");
	uiAtlas.SetAtlasSize(uiShaderProgram, glm::vec2(3, 1));
	uiAtlas.TextureUnit(uiShaderProgram, "tex0");

	// Get mods + mod info (needs to be modularized + actually do things)
	for (const auto& entry : std::filesystem::directory_iterator("Mods")) {
        if (entry.is_directory()) {
            std::string modFolder = entry.path().string();
            std::string modJsonPath = modFolder + "/mod.json";
            
            if (std::filesystem::exists(modJsonPath) && std::filesystem::is_regular_file(modJsonPath)) {
                std::ifstream file(modFolder + "/mod.json");

				json jsonData;
				file >> jsonData;

				std::string name = jsonData["mod_title"];
				std::string version = jsonData["mod_version"];
				std::string authors = jsonData["mod_authors"];

				INFO("Found mod \"" + name + "\" by \"" + authors + "\" with version \"" + version + "\"");
            }
        }
    }

	

	// Setup debug renderer
	Renderer renderer = Renderer();
	DebugRenderer debugRenderer = DebugRenderer();

	UI::GetInstance().Setup(&uiShaderProgram, &uiAtlas);
	UIScreen mainMenuUI = UIScreen("ui/main_menu");
	mainMenuUI.AddUIElement(new UIElement(glm::vec2(100, 100), glm::vec2(64, 16), ""));
	mainMenuUI.AddUIElement(new UIElement(glm::vec2(700, 700), glm::vec2(128, 32), ""));
	mainMenuUI.AddUIElement(new UIElement(glm::vec2(700, 100), glm::vec2(512, 128), "event/generate_world"));
	UI::GetInstance().AddScreen(&mainMenuUI);
	UI::GetInstance().SetCurrentScreen(&mainMenuUI);

	UIScreen settingsUI = UIScreen("ui/settings");
	settingsUI.AddUIElement(new UIElement(glm::vec2(400, 400), glm::vec2(256, 64), ""));
	UI::GetInstance().AddScreen(&settingsUI);

	// Create a singleplayer world
	SingleplayerHandler singleplayerHandler = SingleplayerHandler();
	//singleplayerHandler.singleplayerWorld->GenerateWorld();
	singleplayerHandler.StartSingleplayerWorld(debugRenderer);	
	
	// FPS code
	int frames = 0;
	auto start_time = std::chrono::high_resolution_clock::now();
	double fps = 0.0;

	// Main game loop
	while (!glfwWindowShouldClose(globalWindow.GetWindowInstance())) {
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		auto end_time = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

		if (duration >= 1000.0) {
			fps = static_cast<float>(frames) / (duration / 1000.0);
			frames = 0;
			start_time = end_time;
		}

		ImGui::Begin("Debug");
		ImGui::Text("Total frames: %d", frames);
		ImGui::Text("FPS: %.2f", fps);

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

		textRenderer.RenderText("! i literally am the kiwi and have ui'd this fucking \n game", 300, 600, 1, glm::vec3(frames * 4, 255 - (frames * 4), (frames * 4) / 2));

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(globalWindow.GetWindowInstance());
		++frames;
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