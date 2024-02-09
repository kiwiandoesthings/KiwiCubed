// In loving memory of Riley.
// ~2008-2/8/2024
// R.I.P.




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
#include <Texture.h>
#include <VertexBufferObject.h>
#include <VertexArrayObject.h>
#include <Window.h>


Player player(20, 50, 20);
// Create needed variables
int windowWidth = 600;
int windowHeight = 600;
const char* windowTitle = "Voxel Engine Test";
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{

	// Initialize GLFW
	if (!glfwInit())
	{
		std::cerr << "Initialization / Error: Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Create window
	Window window(windowWidth, windowHeight, windowTitle, framebuffer_size_callback);

	// Initialize GLAD
	gladLoadGL();

	// Set things up before main game loop
	glfwGetFramebufferSize(window.GetWindowInstance(), &windowWidth, &windowHeight);
	glViewport(0, 0, 600, 600);

	Shader shaderProgram("Resources/Shaders/Vertex.vert", "Resources/Shaders/Fragment.frag");

	Chunk myChunk(0, 0, 0);  // Create an instance of Chunk
	Chunk myChunk2(1, 0, 0);  // Create an instance of Chunk

	//Texture test_img("Resources/Textures/Blocks/test_img.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	Texture test_img("Resources/Textures/Blocks/test_img_alt.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);

	glEnable(GL_DEPTH_TEST);

	// Main game loop
	while (!glfwWindowShouldClose(window.GetWindowInstance()))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.8f, 0.4f, 0.6f, 1.0f);

		shaderProgram.Bind();

		player.Update(window.GetWindowInstance(), shaderProgram, "camMatrix", 0, 0, 2);

		test_img.Bind();
		glUniform3f(glGetUniformLocation(shaderProgram.shaderProgramID, "chunkPosition"), myChunk.chunkX, myChunk.chunkY, myChunk.chunkZ);
		myChunk.Render();
		glUniform3f(glGetUniformLocation(shaderProgram.shaderProgramID, "chunkPosition"), myChunk2.chunkX, myChunk2.chunkY, myChunk2.chunkZ);
		myChunk2.Render();

		glfwSwapBuffers(window.GetWindowInstance());

		glfwPollEvents();
	}

	// Clean up once the program has exited
	test_img.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(window.GetWindowInstance());
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	player.UpdateWindowSize(width, height);
}