// In loving memory of Riley.
// ~2008-2/8/2024
// R.I.P.


bool bitness;

// Make it so on laptops, it will request the dGPU if possible, without this, you have to force it to use the dGPU
#ifdef __linux__
// For NVIDIA GPUs
std::setenv("__NV_PRIME_RENDER_OFFLOAD", "1", 1);
std::setenv("__GLX_VENDOR_LIBRARY_NAME", "nvidia", 1);

// For AMD GPUs
std::setenv("DRI_PRIME", "1", 1);
#elif _WIN32
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; // For NVIDIA GPUs
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;   // For AMD GPUs
}
#endif


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <nlohmann/json.hpp>

#include "DebugRenderer.h"
#include "Input.h"
#include "Renderer.h"
#include "Shader.h"
#include "SingleplayerHandler.h"
#include "Texture.h"
#include "Window.h"

// chunk numbers
// says generatede 360 chunks
// 343 chunks is perfect cube
// debug renderer for ALL chunks? super hard without one


using json = nlohmann::json;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);


int windowWidth;
int windowHeight;
std::string windowTitle;
std::string windowType;
std::string projectVersion;

// This main function is getting out of hand
int main() {
	std::ifstream file("Resources/Data/init_config.json");

	if (!file.is_open()) {
		std::cerr << "[Initialization / Error] Could not open or find the JSON config file" << std::endl;
		return 1;
	}

	json jsonData;
	file >> jsonData;

	windowWidth = jsonData["init_settings"]["window_width"];
	windowHeight = jsonData["init_settings"]["window_height"];
	windowTitle = jsonData["init_settings"]["window_title"].get<std::string>();
	windowType = jsonData["init_settings"]["window_type"].get<std::string>();
	projectVersion = jsonData["project_version"].get<std::string>();

	// Initialize GLFW
	if (!glfwInit())
	{
		std::cerr << "[Initialization / Error] Failed to initialize GLFW" << std::endl;
		return -1;
	}
	else {
		std::cout << "[Initialization / Info] Successfully initialized GLFW" << std::endl;
	}

	// Create a window
	Window globalWindow;
	globalWindow.CreateWindowInstance(windowWidth, windowHeight, windowTitle + projectVersion, windowType);
	globalWindow.Setup();

	glfwSetWindowUserPointer(globalWindow.GetWindowInstance(), &globalWindow);
	glfwSetFramebufferSizeCallback(globalWindow.GetWindowInstance(), framebuffer_size_callback);

	// Initialize glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "[Initialization / Error] Failed to initialize GLAD" << std::endl << std::endl;
		return -1;
	}
	else {
		std::cout << "[Initialization / Info] Successfully initialized GLAD" << std::endl << std::endl;
	}

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

	if (sizeof(void*) == 8) {
		bitness = 1;
	}
	else if (sizeof(void*) == 4) {
		bitness = 0;
	}
	else {
		std::cerr << "[Initialization / Error]: Could not find machine bitness" << std::endl;
		return -1;
	}
	std::cout << "[Initialization / Info] Machine bitness: " << (bitness == 1 ? "64" : "32") << std::endl;
	std::cout << "[Initialization / Info] Using OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "[Initialization / Info] Using graphics device: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "[Initialization / Info] Using resolution: " << globalWindow.GetWidth() << " x " << globalWindow.GetHeight() << std::endl << std::endl;


	// Set things up before main game loop
	GLCall(glViewport(0, 0, globalWindow.GetWidth(), globalWindow.GetHeight()));
	 GLCall(glEnable(GL_DEPTH_TEST));

	// Create a singleplayer world
	SingleplayerHandler singleplayerHandler(globalWindow);
	singleplayerHandler.Setup();
	singleplayerHandler.StartSingleplayerWorld();

	// Create a debug shader
	Shader terrainShaderProgram("Resources/Shaders/Terrain_Vertex.vert", "Resources/Shaders/Terrain_Fragment.frag");
	Shader wireframeShaderProgram("Resources/Shaders/Wireframe_Vertex.vert", "Resources/Shaders/Wireframe_Fragment.frag");
	Shader chunkDebugShaderProgram("Resources/Shaders/ChunkDebug_Vertex.vert", "Resources/Shaders/ChunkDebug_Fragment.frag");

	// Create a debug texture
	Texture terrainAtlas("Resources/Textures/Blocks/stone.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	terrainAtlas.SetAtlasSize(terrainShaderProgram, 2);

	singleplayerHandler.singleplayerWorld.GenerateWorld();
	singleplayerHandler.singleplayerWorld.SetupRenderComponents();

	terrainAtlas.Bind();

	Renderer renderer = Renderer();
	DebugRenderer debugRenderer = DebugRenderer();

	glm::vec3 c1 = singleplayerHandler.singleplayerWorld.player.GetEntityData().physicsBoundingBox.corner1;
	glm::vec3 c2 = singleplayerHandler.singleplayerWorld.player.GetEntityData().physicsBoundingBox.corner2;
	glm::vec3 pos = singleplayerHandler.singleplayerWorld.player.GetEntityData().position;

	debugRenderer.SetupBuffers(c1, c2, pos, singleplayerHandler.singleplayerWorld.chunkDebugVisualizationVertices, singleplayerHandler.singleplayerWorld.chunkDebugVisualizationIndices, singleplayerHandler.singleplayerWorld.chunkOrigins);

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
			fps = (float)frames / (duration / 1000.0);
			frames = 0;
			start_time = end_time;
		}

		EntityData playerData = singleplayerHandler.singleplayerWorld.player.GetEntityData();
		ImGui::Begin("Debug");
		if (ImGui::CollapsingHeader("Player Info")) {
			ImGui::Text("Player name: %s", singleplayerHandler.singleplayerWorld.player.GetEntityData().name);
			ImGui::Text("Player health: %d", static_cast<int>(singleplayerHandler.singleplayerWorld.player.GetEntityStats().health));
			ImGui::Text("Player position: %f, %f, %f", 
				playerData.position.x, 
				playerData.position.y, 
				playerData.position.z);
			ImGui::Text("Player orientation: %f, %f, %f", 
				playerData.orientation.x,
				playerData.orientation.y, 
				playerData.orientation.z);
			ImGui::Text("Player velocity: %f, %f, %f", 
				singleplayerHandler.singleplayerWorld.player.GetEntityData().velocity.x, 
				singleplayerHandler.singleplayerWorld.player.GetEntityData().velocity.y, 
				singleplayerHandler.singleplayerWorld.player.GetEntityData().velocity.z);
			ImGui::Text("Global chunk position: %d, %d, %d", 
				static_cast<int>(singleplayerHandler.singleplayerWorld.player.GetEntityData().globalChunkPosition.x), 
				static_cast<int>(singleplayerHandler.singleplayerWorld.player.GetEntityData().globalChunkPosition.y), 
				static_cast<int>(singleplayerHandler.singleplayerWorld.player.GetEntityData().globalChunkPosition.z));
			ImGui::Text("Local chunk position: %d, %d, %d", 
				static_cast<int>(singleplayerHandler.singleplayerWorld.player.GetEntityData().localChunkPosition.x), 
				static_cast<int>(singleplayerHandler.singleplayerWorld.player.GetEntityData().localChunkPosition.y), 
				static_cast<int>(singleplayerHandler.singleplayerWorld.player.GetEntityData().localChunkPosition.z));
			ImGui::Text("Current chunk generation status and blocks %d, %d, %d",
				singleplayerHandler.singleplayerWorld.GetChunk(
					singleplayerHandler.singleplayerWorld.player.GetEntityData().globalChunkPosition.x,
					singleplayerHandler.singleplayerWorld.player.GetEntityData().globalChunkPosition.y,
					singleplayerHandler.singleplayerWorld.player.GetEntityData().globalChunkPosition.z).generationStatus,
				singleplayerHandler.singleplayerWorld.GetChunk(
					singleplayerHandler.singleplayerWorld.player.GetEntityData().globalChunkPosition.x,
					singleplayerHandler.singleplayerWorld.player.GetEntityData().globalChunkPosition.y,
					singleplayerHandler.singleplayerWorld.player.GetEntityData().globalChunkPosition.z).GetTotalBlocks(),
				singleplayerHandler.singleplayerWorld.GetChunk(
					singleplayerHandler.singleplayerWorld.player.GetEntityData().globalChunkPosition.x,
					singleplayerHandler.singleplayerWorld.player.GetEntityData().globalChunkPosition.y,
					singleplayerHandler.singleplayerWorld.player.GetEntityData().globalChunkPosition.z).id);
			ImGui::Text("Total frames: %d", frames);													
			ImGui::Text("FPS: %.2f", fps);
		}

		if (ImGui::CollapsingHeader("World")) {
			singleplayerHandler.singleplayerWorld.DisplayImGui(0);
		}

		if (ImGui::CollapsingHeader("Chunk Info")) {
			singleplayerHandler.singleplayerWorld.DisplayImGui(1);
		}

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

			debugRenderer.UpdateBuffers(c1, c2, pos, singleplayerHandler.singleplayerWorld.chunkDebugVisualizationVertices, singleplayerHandler.singleplayerWorld.chunkDebugVisualizationIndices, singleplayerHandler.singleplayerWorld.chunkOrigins);
			debugRenderer.UpdateUniforms();
			debugRenderer.RenderDebug(wireframeShaderProgram, chunkDebugShaderProgram);
		}


		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(globalWindow.GetWindowInstance());
		++frames;
	}

	singleplayerHandler.EndSingleplayerWorld();

	// Clean up once the program has exited
	std::cout << "[Cleanup / Info] Cleaned up, exiting program" << std::endl;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(globalWindow.GetWindowInstance());
	glfwTerminate();

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