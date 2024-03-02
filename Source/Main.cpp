// In loving memory of Riley.
// ~2008-2/8/2024
// R.I.P.
// 
// 
// 
//
// 
// 
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <string>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <stb/stb_image.h>

#include <Block.h>
#include <Camera.h>
#include <Chunk.h>
#include <IndexBufferObject.h>
#include <Player.h>
#include <Shader.h>
#include <SingleplayerHandler.h>
#include <Texture.h>
#include <VertexBufferObject.h>
#include <VertexArrayObject.h>
#include <Window.h>
#include <World.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int windowWidth = 640;
int windowHeight = 480;
const char* windowTitle = "Voxel Engine Test";

// Create a window
Window globalWindow = Window(windowWidth, windowHeight, windowTitle, framebuffer_size_callback);

int main() {

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Initialization / Error: Failed to initialize GLAD" << std::endl;
		return -1;
	}
	else {
		std::cout << "Initialization / Info: Successfully initialized GLAD" << std::endl;
	}


	// Set things up before main game loop
	windowWidth = globalWindow.GetWidth();
	windowHeight = globalWindow.GetHeight();
	glViewport(0, 0, windowWidth, windowHeight);
	
	// Create a singleplayer world
	SingleplayerHandler singleplayerHandler = SingleplayerHandler();
	singleplayerHandler.StartSingleplayerWorld();

	// Create a debug shader
	Shader shaderProgram("Resources/Shaders/Vertex.vert", "Resources/Shaders/Fragment.frag");
	// Create a debug texture
	Texture test_img("Resources/Textures/Blocks/test_img_alt.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	glEnable(GL_DEPTH_TEST);
	
	//singleplayerHandler.singlePlayerWorld.GenerateChunk(0, 0, 0);
	
	// Main game loop
	while (!glfwWindowShouldClose(globalWindow.GetWindowInstance())) {
		// Make background ~pink~
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.8f, 0.4f, 0.6f, 1.0f);
		
		// Do rendering stuff
		shaderProgram.Bind();
		test_img.Bind();
		globalWindow.Inputs();
		if (singleplayerHandler.isLoadedIntoSingleplayerWorld) {
			singleplayerHandler.singlePlayerWorld.player.Update(globalWindow.GetWindowInstance(), globalWindow, shaderProgram, "worldViewMatrix", 0, 0, 0);
		}

		std::cout << std::get<0>(singleplayerHandler.singlePlayerWorld.player.GetPosition()) << " " << std::get<1>(singleplayerHandler.singlePlayerWorld.player.GetPosition()) << " " << std::get<2>(singleplayerHandler.singlePlayerWorld.player.GetPosition()) << std::endl;

		//singleplayerHandler.singlePlayerWorld.Render();

		// Do GLFW crap
		glfwSwapBuffers(globalWindow.GetWindowInstance());
		glfwPollEvents();
	}

	// Clean up once the program has exited
	test_img.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(globalWindow.GetWindowInstance());
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	globalWindow.UpdateWindowSize(width, height);
}