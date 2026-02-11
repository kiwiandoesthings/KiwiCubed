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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#include "Events.h"
#include "DebugRenderer.h"
#include "Globals.h"
#include "Input.h"
#include "KiwiCubed.h"
#include "ModHandler.h"
#include "Renderer.h"
#include "Shader.h"
#include "SingleplayerHandler.h"
#include "TextRenderer.h"
#include "Texture.h"
#include "UI.h"
#include "Window.h"


using OrderedJSON = nlohmann::ordered_json;


unsigned char bitness;


// TODO: stuff needs restructuring because game loading is very order-dependent, and if this keeps
// up, its gonna become order-dependent in impossible ways so that should be redone asap really
int main(int argc, char* argv[]) {
	OVERRIDE_LOG_NAME("Initialization");
	// Make it so on laptops, it will request the dGPU if possible, without this, you have to force it to use the dGPU
	#ifdef __linux__
	// For NVIDIA GPUs
	setenv("__NV_PRIME_RENDER_OFFLOAD", "1", 1);
	setenv("__GLX_VENDOR_LIBRARY_NAME", "nvidia", 1);

	// For AMD GPUs
	setenv("DRI_PRIME", "1", 1);
	#endif

	KiwiCubedEngine kiwiCubed;
	for (int iterator = 0; iterator < argc; iterator++) {
		if (argv[iterator] == "-novalidate") {
			INFO("Configuration validation disabled for this session");
			kiwiCubed.GetGlobals().validateConfig = false;
		}
	}
	LOG_CHECK_CRITICAL(kiwiCubed.StartEngine() != 0, "Successfully initialized KiwiCubed Engine", "Error returned while initializing KiwiCubed Engine", -1);
	LOG_CHECK_RETURN_BAD_CRITICAL(kiwiCubed.RunGameLoop(), "Error returned while running game loop, aborting", -1);

	return 0;
}