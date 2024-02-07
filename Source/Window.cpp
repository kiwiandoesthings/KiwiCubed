#include <Window.h>

//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
Window::Window(int windowWidth, int windowHeight, const char* windowTitle) {
	Window::windowWidth = windowWidth;
	Window::windowHeight = windowHeight;
	Window::windowTitle = windowTitle;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create GLFW window
	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Initialization / Error: Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwMakeContextCurrent(window);
}

int Window::GetWidth() {
	return Window::windowWidth;
}

int Window::GetHeight() {
	return Window::windowHeight;
}

const char* Window::GetTitle() {
	return Window::windowTitle;
}

GLFWwindow* Window::GetWindowInstance() {
	return Window::window;
}

Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

//void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//	glViewport(0, 0, width, height);
//	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
//	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
//	//camera.UpdateWindowSize(width, height);
//}