#include "Window.h"


Window::Window(int windowWidth, int windowHeight, const char* windowTitle) : isFocused(false), window(glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr)) {
	Window::windowWidth = windowWidth;
	Window::windowHeight = windowHeight;
	Window::windowTitle = windowTitle;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GL_CONTEXT_PROFILE_MASK, 24);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	int newWindowWidth, newWindowHeight;
	glfwGetFramebufferSize(window, &newWindowWidth, &newWindowHeight);
	Window::windowWidth = newWindowWidth;
	Window::windowHeight = newWindowHeight;

	if (!window)
	{
		std::cerr << "[Initialization / Error] Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	else {
		std::cout << "[Initialization / Info] Successfully created GLFW window" << std::endl;
	}
	glfwMakeContextCurrent(window);

	inputHandler.SetupCallbacks(window);
}

Window::Window(int windowWidth, int windowHeight, std::string windowTitle) : isFocused(false), window(glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), nullptr, nullptr)) {
	Window::windowWidth = windowWidth;
	Window::windowHeight = windowHeight;
	Window::windowTitle = windowTitle.c_str();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GL_CONTEXT_PROFILE_MASK, 24);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	int newWindowWidth, newWindowHeight;
	glfwGetFramebufferSize(window, &newWindowWidth, &newWindowHeight);
	Window::windowWidth = newWindowWidth;
	Window::windowHeight = newWindowHeight;

	if (!window)
	{
		std::cerr << "[Initialization / Error] Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	else {
		std::cout << "[Initialization / Info] Successfully created GLFW window" << std::endl;
	}
	glfwMakeContextCurrent(window);

	inputHandler.SetupCallbacks(window);
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