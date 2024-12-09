#include "Window.h"
#include "GLFW/glfw3.h"


Window::Window() {
}

GLFWwindow* Window::CreateWindowInstance(int windowWidth, int windowHeight, const char* windowTitle, const char* windowType) {
	Window::windowWidth = windowWidth;
	Window::windowHeight = windowHeight;
	Window::windowTitle = windowTitle;

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	if (strcmp(windowType, "fullscreen") == 0) {
		window = glfwCreateWindow(mode->width, mode->height, windowTitle, monitor, nullptr);
	}
	else if (strcmp(windowType, "window_borderless") == 0) {
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		window = glfwCreateWindow(mode->width, mode->height, windowTitle, nullptr, nullptr);
	}
	else if (strcmp(windowType, "window_maximized") == 0) {
		window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
		glfwMaximizeWindow(window);
	}
	else if (strcmp(windowType, "window") == 0) {
		window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GL_CONTEXT_PROFILE_MASK, 24);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	if (!window)
	{
		std::cerr << "[Initialization / Error] Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	else {
		std::cout << "[Initialization / Info] Successfully created GLFW window" << std::endl;
	}

	int newWindowWidth;
	int newWindowHeight;

	glfwGetWindowSize(window, &newWindowWidth, &newWindowHeight);

	Window::windowWidth = newWindowWidth;
	Window::windowHeight = newWindowHeight;

	glfwMakeContextCurrent(window);

	inputHandler.SetupCallbacks(window);

	return window;
}

GLFWwindow* Window::CreateWindowInstance(int windowWidth, int windowHeight, std::string windowTitle, std::string windowType) {
	Window::windowWidth = windowWidth;
	Window::windowHeight = windowHeight;
	Window::windowTitle = windowTitle.c_str();

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	if (windowType == "fullscreen") {
		window = glfwCreateWindow(mode->width, mode->height, windowTitle.c_str(), monitor, nullptr);
	}
	else if (windowType == "window_borderless") {
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		window = glfwCreateWindow(mode->width, mode->height, windowTitle.c_str(), nullptr, nullptr);
	}
	else if (windowType == "window_maximized") {
		window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), nullptr, nullptr);
		glfwMaximizeWindow(window);
	}
	else if (windowType == "window") {
		window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), nullptr, nullptr);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GL_CONTEXT_PROFILE_MASK, 24);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	if (!window)
	{
		std::cerr << "[Initialization / Error] Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	else {
		std::cout << "[Initialization / Info] Successfully created GLFW window" << std::endl;
	}

	int newWindowWidth;
	int newWindowHeight;

	glfwGetWindowSize(window, &newWindowWidth, &newWindowHeight);

	Window::windowWidth = newWindowWidth;
	Window::windowHeight = newWindowHeight;

	glfwMakeContextCurrent(window);

	inputHandler.SetupCallbacks(window);

	return window;
}

void Window::Setup() {
	inputHandler.RegisterKeyCallback(std::vector<int>{GLFW_KEY_ESCAPE}, [&]() {
		isFocused = false;
	});
	inputHandler.RegisterKeyCallback({GLFW_KEY_SPACE}, [&]() {
		isFocused = true;
		glfwSetCursorPos(window, static_cast<float>(windowWidth) / 2, static_cast<float>(windowHeight) / 2);
	});
}

void Window::QueryInputs() {
	if (!isFocused) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		return;
	}
	if (isFocused) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_CURSOR_HIDDEN);
	}
}

void Window::UpdateWindowSize(int newWindowWidth, int newWindowHeight) {
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;
}

int Window::GetWidth() const {
	return windowWidth;
}

int Window::GetHeight() const {
	return windowHeight;
}

const char* Window::GetTitle() const {
	return windowTitle;
}

void Window::SetTitle(const char* newTitle) {
	glfwSetWindowTitle(window, newTitle);
	windowTitle = newTitle;
}

void Window::SetTitle(std::string newTitle) {
	glfwSetWindowTitle(window, newTitle.c_str());
	windowTitle = newTitle.c_str();
}

GLFWwindow* Window::GetWindowInstance() {
	return window;
}

void Window::Delete() {
	glfwDestroyWindow(window);
	glfwTerminate();
}