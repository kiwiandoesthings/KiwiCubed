// In loving memory of Riley.
// ~2008-2/8/2024
// R.I.P.

char versionString[128];

extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}


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
	strcpy_s(versionString, windowTitle);
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
	std::cout << "Initialization / Info: Using graphics device: " << glGetString(GL_RENDERER) << std::endl;


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
	Texture textureAtlas("Resources/Textures/Blocks/test_img_alt.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);

	// Start generating a chunk in the singleplayer world
	singleplayerHandler.singlePlayerWorld.GenerateWorld();

	// Bind stuff
	textureAtlas.Bind();
	shaderProgram.Bind();

	Renderer renderer = Renderer();

	InputHandler inputHandler = InputHandler(&globalWindow);

	int frames = 0;

	//GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//glCompileShader(vertexShader);
	//// Check for compile errors here
	//
	//GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	//glCompileShader(fragmentShader);
	//// Check for compile errors here
	//
	//// Link shaders
	//GLuint shaderProgram = glCreateProgram();
	//glAttachShader(shaderProgram, vertexShader);
	//glAttachShader(shaderProgram, fragmentShader);
	//glLinkProgram(shaderProgram);
	//// Check for linking errors here
	//
	//// Clean up shader objects
	//glDeleteShader(vertexShader);
	//glDeleteShader(fragmentShader);
	//
	//GLuint VAOs[2], VBOs[2], EBOs[2];
	//glGenVertexArrays(2, VAOs);
	//glGenBuffers(2, VBOs);
	//glGenBuffers(2, EBOs);
	//
	//// First Object
	//glBindVertexArray(VAOs[0]);
	//glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangleIndices), triangleIndices, GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	//glEnableVertexAttribArray(0);
	//glBindVertexArray(0);

	// Second Object
	//glBindVertexArray(VAOs[1]);
	//glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIndices), squareIndices, GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	//glEnableVertexAttribArray(0);
	//glBindVertexArray(0);

	// Set up draw parameters for both objects
	//GLint drawCount[] = {static_cast<GLint>(sizeof(triangleIndices) / sizeof(GLuint)), static_cast<GLint>(sizeof(squareIndices) / sizeof(GLuint))};
	//GLint drawStart[] = {0, 0};
	//GLint vertexBase[] = {0, 0};
	//
	//// Set up indirect buffer
	//struct DrawElementsIndirectCommand {
	//	GLuint count;
	//	GLuint instanceCount;
	//	GLuint firstIndex;
	//	GLuint baseVertex;
	//	GLuint baseInstance;
	//};
	//DrawElementsIndirectCommand commands[2];
	//commands[0].count = static_cast<GLuint>(sizeof(triangleIndices) / sizeof(GLuint));
	//commands[0].instanceCount = 1;
	//commands[0].firstIndex = 0;
	//commands[0].baseVertex = 0;
	//commands[0].baseInstance = 0;
	//commands[1].count = static_cast<GLuint>(sizeof(squareIndices) / sizeof(GLuint));
	//commands[1].instanceCount = 1;
	//commands[1].firstIndex = 0;
	//commands[1].baseVertex = 0;
	//commands[1].baseInstance = 0;
	//
	//GLuint indirectBuffer;
	//glGenBuffers(1, &indirectBuffer);
	//glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
	//glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(commands), commands, GL_STATIC_DRAW);
	//glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

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
		//glUseProgram(shaderProgram);
		//
		//// Draw both objects using glMultiDrawElementsIndirect
		//glBindVertexArray(VAOs[0]);
		//glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
		//glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, 2, 0);
		//glBindVertexArray(0);


		// Do GLFW crap
		glfwSwapBuffers(globalWindow.GetWindowInstance());
		glfwPollEvents();

		++frames;
	}

	// Clean up once the program has exited
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