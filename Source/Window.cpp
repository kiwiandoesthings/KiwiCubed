#include <Window.h>


Window::Window(int windowWidth, int windowHeight, const char* windowTitle, GLFWframebuffersizefun callback) {
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
	glfwSetFramebufferSizeCallback(window, callback);
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