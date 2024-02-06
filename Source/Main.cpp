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
#include <Shader.h>
#include <Texture.h>
#include <VertexBufferObject.h>
#include <VertexArrayObject.h>

// Create needed variables
int windowWidth = 600;
int windowHeight = 600;
const char* windowTitle = "Voxel Engine Test";
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

int main()
{

	// Initialize GLFW
	if (!glfwInit())
	{
		std::cerr << "Initialization / Error: Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Give GLFW hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create GLFW window
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Initialization / Error: Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//glfwSetWindowSizeCallback(window, window_size_callback);

	// Initialize GLAD
	gladLoadGL();

	// Set things up before main game loop
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Chunk myChunk(0, 0, 0);  // Create an instance of Chunk
	//Chunk myChunk2(1, 0, 0);  // Create an instance of Chunk

	// Create and bind shaders
	Shader shaderProgram("Resources/Shaders/Vertex.vert", "Resources/Shaders/Fragment.frag");

	GLfloat vertices[] = {
		// Positions          // Texture Coordinates
		// Front
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,   0.0f, 1.0f,

		// Back
		-0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,   1.0f, 1.0f,

		// Left
		-0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f,

		// Right
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,   1.0f, 0.0f,

		 // Top
		 -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,
		  0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
		  0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
		 -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,

		 // Bottom
		 -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
		  0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
		  0.5f, -0.5f, -0.5f,   1.0f, 1.0f,
		 -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,
	};

	GLuint indices[] = {
		0, 1, 2,  // Front
		2, 3, 0,

		4, 5, 6,  // Back
		6, 7, 4,

		8, 9, 10,  // Left
		10, 11, 8,

		12, 13, 14,  // Right
		14, 15, 12,

		16, 17, 18,  // Top
		18, 19, 16,

		20, 21, 22,  // Bottom
		22, 23, 20,
	};

	//VertexArrayObject vertexArrayObject;
	//vertexArrayObject.Bind();
	//VertexBufferObject vertexBufferObject(vertices, sizeof(vertices));
	//IndexBufferObject indexBufferObject(indices, sizeof(indices));
	//vertexArrayObject.LinkAttribute(vertexBufferObject, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	////vertexArrayObject.LinkAttribute(vertexBufferObject, 1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//vertexArrayObject.LinkAttribute(vertexBufferObject, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	//vertexArrayObject.Unbind();
	//vertexBufferObject.Unbind();
	//indexBufferObject.Unbind();

	Texture test_img("Resources/Textures/Blocks/test_img.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);

	glEnable(GL_DEPTH_TEST);

	// Main game loop
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.8f, 0.4f, 0.6f, 1.0f);

		shaderProgram.Bind();

		camera.Inputs(window);
		camera.UpdateMatrix(80.0f, 0.1f, 1000.0f);
		camera.Matrix(shaderProgram, "camMatrix");

		test_img.Bind();
		//vertexArrayObject.Bind();
		//glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(float), GL_UNSIGNED_INT, 0);

		myChunk.Render();
		//myChunk2.Render();

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// Clean up once the program has exited
	//vertexArrayObject.Delete();
	//vertexBufferObject.Delete();
	//indexBufferObject.Delete();
	test_img.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	camera.UpdateWindowSize(width, height);
}