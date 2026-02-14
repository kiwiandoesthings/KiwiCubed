#include "KiwiCubed.h"


void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void WindowFocusCallback(GLFWwindow* window, int focused);


KiwiCubedEngine::KiwiCubedEngine() {}

int KiwiCubedEngine::StartEngine() {
	OVERRIDE_LOG_NAME("Initialization");
    INFO("Running KiwiCubed version {" + globals.projectVersion + "}");
	if (globals.debugMode) {
		INFO("Running a debug KiwiCubed build");
	}

	// Initialize GLFW
	LOG_CHECK_RETURN_CRITICAL(glfwInit(), "Successfully initialized GLFW", "Failed to initialize GLFW, aborting", -1);

	// Create a window
	GLFWCallbacks callbacks = GLFWCallbacks{
		FramebufferSizeCallback,
		WindowFocusCallback
	};
	glfwWindow = globalWindow.CreateGLFWWindow(globals.windowWidth, globals.windowHeight, std::string(globals.windowTitle + globals.projectVersion).c_str(), globals.windowType.c_str(), callbacks);

    // Initialize glad
	LOG_CHECK_RETURN_CRITICAL(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Successfully initialized GLAD", "Failed to initialize GLAD, aborting", -1);

	// Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	io.IniFilename = NULL; // don't save the accursed imgui.ini
	ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
	ImGui_ImplOpenGL3_Init("#version 430");

	// Setup window *after* ImGui is initialized
	globalWindow.Setup();

    // System info
	if (sizeof(void*) == 8) {
		globals.bitness = 64;
	} else if (sizeof(void*) == 4) {
		globals.bitness = 32;
	} else {
		CRITICAL("Bitness check failed somehow, aborting");
		psnip_trap();
	}
	INFO("Machine bitness: " + std::to_string(globals.bitness));
	INFO("Using OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));
	INFO("Using graphics device: " + std::string((char*)glGetString(GL_RENDERER)));
	INFO("Using resolution: " + std::to_string(globalWindow.GetWidth()) + " x " + std::to_string(globalWindow.GetHeight()));

	// Set things up before main game loop
	GLCall(glViewport(0, 0, globalWindow.GetWidth(), globalWindow.GetHeight()));
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glCullFace(GL_FRONT));

	//eventManager.RegisterEvent("event/blank");

	// Initialize FreeType
	FT_Library ft;
    LOG_CHECK_RETURN_CRITICAL(!FT_Init_FreeType(&ft), "Successfully initialized FreeType", "Failed to initialize FreeType, exiting", -1);
    FT_Face face;
    LOG_CHECK_RETURN_CRITICAL(!FT_New_Face(ft, "Mods/kiwicubed/Resources/Fonts/PixiFont.ttf", 0, &face), "Successfully loaded font PixiFont.ttf", "Failed to load font PixiFont.ttf, exiting", -1);
    FT_Set_Pixel_Sizes(face, 0, 32);

	std::unique_ptr<Shader> textShader = std::make_unique<Shader>("Mods/kiwicubed/Resources/Shaders/Text_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/Text_Fragment.frag");
	assetManager.RegisterShaderProgram({"kiwicubed", "text_shader"}, *(textShader.get()));

	textRenderer.Setup(ft, face, AssetStringID{"kiwicubed", "text_shader"});

	// MAIN PROGRAM SETUP FINISHED - Most of the rest of this is able to be moved into other places to make this more modular
	
	// Create shader programs (needs to be modularized)
	terrainShaderProgram = new Shader("Mods/kiwicubed/Resources/Shaders/Terrain_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/Terrain_Fragment.frag");
	assetManager.RegisterShaderProgram({"kiwicubed", "terrain_shader"}, *terrainShaderProgram);
	wireframeShaderProgram = new Shader("Mods/kiwicubed/Resources/Shaders/Wireframe_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/Wireframe_Fragment.frag");
	assetManager.RegisterShaderProgram({"kiwicubed", "wireframe_shader"}, *wireframeShaderProgram);
	chunkDebugShaderProgram = new Shader("Mods/kiwicubed/Resources/Shaders/ChunkDebug_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/ChunkDebug_Fragment.frag");
	assetManager.RegisterShaderProgram({"kiwicubed", "chunk_debug_shader"}, *chunkDebugShaderProgram);
	uiShaderProgram = new Shader("Mods/kiwicubed/Resources/Shaders/UI_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/UI_Fragment.frag");
	assetManager.RegisterShaderProgram({"kiwicubed", "ui_shader"}, *uiShaderProgram);
	entityShaderProgram = new Shader("Mods/kiwicubed/Resources/Shaders/Entity_Vertex.vert", "Mods/kiwicubed/Resources/Shaders/Entity_Fragment.frag");
	assetManager.RegisterShaderProgram({"kiwicubed", "entity_shader"}, *entityShaderProgram);

	// Create texture atlases (needs to be modularized)
	terrainAtlas = new Texture("Mods/kiwicubed/Resources/Textures/terrain_atlas.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE, "texture/terrain");
	terrainAtlas->SetAtlasSize(*chunkDebugShaderProgram, glm::vec2(4, 4));
	terrainAtlas->TextureUnit(*terrainShaderProgram, "tex0");
	terrainAtlas->TextureUnit(*chunkDebugShaderProgram, "tex0");
	assetManager.RegisterTextureAtlas({"kiwicubed", "terrain_atlas"}, *terrainAtlas);
	Texture uiAtlas("Mods/kiwicubed/Resources/Textures/ui_atlas.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE, "texture/gui");
	uiAtlas.SetAtlasSize(*uiShaderProgram, glm::vec2(12, 12));
	uiAtlas.TextureUnit(*uiShaderProgram, "tex0");
	assetManager.RegisterTextureAtlas({"kiwicubed", "ui_atlas"}, uiAtlas);

	// Mods
	LOG_CHECK_RETURN_BAD_CRITICAL(modHandler.LoadModData(), "Failed to load mod data, exiting", -1);
	LOG_CHECK_RETURN_BAD_CRITICAL(modHandler.LoadModScripts(), "Failed to load mod scripts, exiting", -1);

	// "temporary" setup of certain events in main
	//eventManager.RegisterEvent("event/close_game");
	//eventManager.AddEventToDo("event/close_game", [&](Event& event) {
	//	glfwSetWindowShouldClose(glfwWindow, true);
	//});
	//eventManager.RegisterEvent("event/settings/change_fov");
	//eventManager.AddEventToDo("event/settings/change_fov", [&](Event& event) {
	//	OVERRIDE_LOG_NAME("Settings");
	//	OrderedJSON configJSON = globals.GetConfigJSON();
	//
	//	globals.fov += 10;
	//	if (globals.fov > 120) {
	//		globals.fov = 30;
	//	}
	//	configJSON["init_settings"]["fov"] = globals.fov;
	//	std::ofstream configWrite("init_config.json");
	//	if (!configWrite.is_open()) {
	//		CRITICAL("Could not open the JSON config file, aborting");
	//		psnip_trap();
	//	}
	//	configWrite << configJSON.dump(1, '\t');
	//	configWrite.close();
	//
	//	if (!singleplayerHandler.IsLoadedIntoSingleplayerWorld()) {
	//		return;
	//	}
	//
	//	Player& player = singleplayerHandler.GetWorld()->GetPlayer();
	//	player.fov = globals.fov;
	//});

	ui.Setup(*uiShaderProgram, uiAtlas, &textRenderer);
	UIScreen mainMenuUI = UIScreen("ui/main_menu");
	mainMenuUI.AddUIElement(new UIButton(glm::vec2((globalWindow.GetWidth() / 2) - 256, 700), glm::vec2(1, 1), [&]() {
		//eventManager.TriggerEvent("event/generate_world");
		if (!singleplayerHandler.IsLoadedIntoSingleplayerWorld()) {
			singleplayerHandler.StartSingleplayerWorld();
		}
	}, "Create World"));
	mainMenuUI.AddUIElement(new UIButton(glm::vec2((globalWindow.GetWidth() / 2) - 256, 500), glm::vec2(1, 1), [&]() {
		ui.SetCurrentScreen("ui/settings");
	}, "Settings"));
	mainMenuUI.AddUIElement(new UIButton(glm::vec2((globalWindow.GetWidth() / 2) - 256, 300), glm::vec2(1, 1), [&]() {
		//eventManager.TriggerEvent("event/close_game");
	}, "Exit"));
	ui.AddScreen(std::move(mainMenuUI));
	ui.SetCurrentScreen("ui/main_menu");

	UIScreen settingsUI = UIScreen("ui/settings");
	settingsUI.AddUIElement(new UIButton(glm::vec2((globalWindow.GetWidth() / 2) - 256, 700), glm::vec2(1, 1), [&]() {
		//eventManager.TriggerEvent("event/settings/change_fov");
	}, "Change FOV"));
	settingsUI.AddUIElement(new UIButton(glm::vec2((globalWindow.GetWidth() / 2) - 256, 500), glm::vec2(1, 1), [&]() {
		ui.MoveScreenBack();
	}, "Back"));
	ui.AddScreen(std::move(settingsUI));

	UIScreen gamePauseUI = UIScreen("ui/game_pause");
	gamePauseUI.AddUIElement(new UIButton(glm::vec2((globalWindow.GetWidth() / 2) - 256, 700), glm::vec2(1, 1), [&]() {
		ui.DisableUI();
	}, "Resume game"));
	gamePauseUI.AddUIElement(new UIButton(glm::vec2((globalWindow.GetWidth() / 2) - 256, 500), glm::vec2(1, 1), [&]() {
		ui.SetCurrentScreen("ui/settings");
	}, "Settings"));
	gamePauseUI.AddUIElement(new UIButton(glm::vec2((globalWindow.GetWidth() / 2) - 256, 300), glm::vec2(1, 1), [&]() {
		//eventManager.TriggerEvent("event/unload_world");
	}, "Quit Game"));
	ui.AddScreen(std::move(gamePauseUI));

	return 0;
}

bool KiwiCubedEngine::RunGameLoop() {
	// FPS code
	int frames = 0;
	auto startTime = std::chrono::steady_clock::now();
	double fps = 0.0f;

	// Main game loop
	while (!glfwWindowShouldClose(glfwWindow)) {
		auto frameStartTime = std::chrono::steady_clock::now();
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		auto endTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

		if (duration >= 1000.0f) {
			fps = static_cast<float>(frames) / (duration / 1000.0f);
			frames = 0;
			startTime = endTime;
		}

		static char input[256] = {};

		ImGui::Begin("Debug");
		ImGui::Text("Total frames: %d", frames);
		ImGui::Text("FPS: %.2f", fps);
		ImGui::Text("DeltaTime: %.6f", globals.deltaTime);

		if (singleplayerHandler.IsLoadedIntoSingleplayerWorld()) {
			World* singleplayerWorld = singleplayerHandler.GetWorld();
			if (ImGui::CollapsingHeader("Player")) {
				singleplayerWorld->DisplayImGui(0);
			}

			if (ImGui::CollapsingHeader("World")) {
				singleplayerWorld->DisplayImGui(1);
			}

			if (ImGui::CollapsingHeader("Chunk Info")) {
				singleplayerWorld->DisplayImGui(2);
			}
		}

		// Make background ~pink~
		renderer.ClearScreen(0.98f, 0.88f, 1.0f);
		// Do rendering stuff
		globalWindow.QueryInputs();
		if (singleplayerHandler.IsLoadedIntoSingleplayerWorld()) {
			singleplayerHandler.Update();
		}

		if (singleplayerHandler.IsLoadedIntoSingleplayerWorld()) {
			World* singleplayerWorld = singleplayerHandler.GetWorld();
			singleplayerWorld->GetPlayer().Update();
			singleplayerWorld->GetPlayer().UpdateCameraMatrix(*terrainShaderProgram);
			singleplayerWorld->GetPlayer().UpdateCameraMatrix(*wireframeShaderProgram);
			singleplayerWorld->GetPlayer().UpdateCameraMatrix(*chunkDebugShaderProgram);
			singleplayerWorld->GetPlayer().UpdateCameraMatrix(*entityShaderProgram);
			singleplayerWorld->GetChunkHandler().CleanChunks();
			
			terrainAtlas->SetActive();
			terrainAtlas->Bind();
			debugRenderer.UpdateBuffers(singleplayerWorld->GetChunkDebugVisualizationVertices(), singleplayerWorld->GetChunkDebugVisualizationIndices(), singleplayerWorld->GetChunkOrigins());
			debugRenderer.UpdateUniforms();
			debugRenderer.RenderDebug(*chunkDebugShaderProgram);
			singleplayerWorld->Render(*terrainShaderProgram);
		}
		
		UI::GetInstance().Render();

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(glfwWindow);
		++frames;

		auto frameEndTime = std::chrono::steady_clock::now();
		if (singleplayerHandler.IsLoadedIntoSingleplayerWorld()) {
			singleplayerHandler.GetWorld()->UpdateFrameTime(frameEndTime);
		}
		globals.deltaTime = abs(duration_cast<std::chrono::duration<float>>(frameStartTime - frameEndTime).count());
		globals.frameCount++;
	}

	// Clean up once the program has exited
	OVERRIDE_LOG_NAME("Cleanup");
	INFO("Cleaning up...");
	if (singleplayerHandler.IsLoadedIntoSingleplayerWorld()) {
		singleplayerHandler.EndSingleplayerWorld();
	}

	eventManager.Delete();
	ui.Delete();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	globalWindow.Delete();

	INFO("Finished cleaning up");

	return true;
}

void KiwiCubedEngine::Exit() {

}

Globals& KiwiCubedEngine::GetGlobals() {
	return globals;
}


void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	OVERRIDE_LOG_NAME("GLFW Callback / Framebuffer Size");
	if (width == 0 || height == 0) {
		return;
	}
	glViewport(0, 0, width, height);
	Window* globalWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!globalWindow) {
		ERR("Could not get window instance for GLFW callback");
	}
	globalWindow->UpdateWindowSize(width, height);
}

void WindowFocusCallback(GLFWwindow* window, int focused) {
	OVERRIDE_LOG_NAME("GLFW Callback / Window Focus");
	Window* globalWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!globalWindow) {
		ERR("Could not get window instance for GLFW callback");
	}
	// Disabled until I have a better solution for this
	if (focused) {
		//globalWindow->SetFocused(true);
	} else {
		//globalWindow->SetFocused(false);
	}
}