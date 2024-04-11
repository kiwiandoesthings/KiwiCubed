// In loving memory of Riley.
// ~2008-2/8/2024
// R.I.P.

char versionString[128];



#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <string>
#include <time.h>

#include <Input.h>
#include <Renderer.h>
#include <Shader.h>
#include <SingleplayerHandler.h>
#include <Texture.h>
#include <Window.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);


// TODO: Replace with JSON file loading or something
int windowWidth = 640;
int windowHeight = 480;
const char* windowTitle = "KiwiCubed Engine";


int main() {

	// Initialize GLFW
	if (!glfwInit())
	{
		std::cerr << "Initialization / Error: Failed to initialize GLFW" << std::endl;
		return -1;
	}
	else {
		std::cout << "Initialization / Info: Successfully initialized GLFW" << std::endl;
	}

	// Create a window
	Window globalWindow = Window(windowWidth, windowHeight, windowTitle);
	strcpy_s(versionString, "KiwiCubed Engine ");
	strcat_s(versionString, "v0.0.1pre-alpha");
	globalWindow.SetTitle(versionString);

	glfwSetWindowUserPointer(globalWindow.GetWindowInstance(), &globalWindow);
	glfwSetFramebufferSizeCallback(globalWindow.GetWindowInstance(), framebuffer_size_callback);

	// Initialize glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Initialization / Error: Failed to initialize GLAD" << std::endl;
		return -1;
	}
	else {
		std::cout << "Initialization / Info: Successfully initialized GLAD" << std::endl;
	}

	std::cout << "Initialization / Info: Using OpenGL version: " << glGetString(GL_VERSION) << std::endl;


	// Set things up before main game loop
	windowWidth = globalWindow.GetWidth();
	windowHeight = globalWindow.GetHeight();
	GLCall(glViewport(0, 0, windowWidth, windowHeight));
	GLCall(glEnable(GL_DEPTH_TEST));

	// Create a singleplayer world
	SingleplayerHandler singleplayerHandler = SingleplayerHandler();
	singleplayerHandler.StartSingleplayerWorld();

	// Create a debug shader
	Shader shaderProgram("Resources/Shaders/Vertex.vert", "Resources/Shaders/Fragment.frag");

	// Create a debug texture
	Texture test_img("Resources/Textures/Blocks/test_img_alt.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);

	// Start generating a chunk in the singleplayer world
	singleplayerHandler.singlePlayerWorld.GenerateWorld();

	// Bind stuff
	test_img.Bind();
	shaderProgram.Bind();

	Renderer renderer = Renderer();

	InputHandler inputHandler = InputHandler(&globalWindow);

	int frames = 0;

	// Main game loop
	while (!glfwWindowShouldClose(globalWindow.GetWindowInstance())) {

		// Make background ~pink~
		renderer.ClearScreen(0.98f, 0.88f, 1.0f);

		// Do rendering stuff
		globalWindow.Inputs();
		if (singleplayerHandler.isLoadedIntoSingleplayerWorld) {
			singleplayerHandler.singlePlayerWorld.player.Update(&globalWindow, shaderProgram, "windowViewMatrix", 0, 0, 0);
		}
		
		singleplayerHandler.singlePlayerWorld.Render(shaderProgram);

		// Do GLFW crap
		glfwSwapBuffers(globalWindow.GetWindowInstance());
		glfwPollEvents();

		++frames;
	}

	// Clean up once the program has exited
	test_img.Delete();
	shaderProgram.Delete();
	std::cout << "Cleanup / Info: Cleaned up, exiting program" << std::endl;
	glfwDestroyWindow(globalWindow.GetWindowInstance());
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	Window* globalWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (globalWindow) {
		globalWindow->UpdateWindowSize(width, height);
	}
}