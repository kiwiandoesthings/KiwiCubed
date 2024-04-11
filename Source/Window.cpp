#include <Window.h>


Window::Window(int windowWidth, int windowHeight, const char* windowTitle) : isFocused(false), firstClick(true), window(window) {
	Window::windowWidth = windowWidth;
	Window::windowHeight = windowHeight;
	Window::windowTitle = windowTitle;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create GLFW window
	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Initialization / Error: Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	else {
		std::cout << "Initialization / Info: Successfully created GLFW window" << std::endl;
	}
	glfwMakeContextCurrent(window);
}

void Window::Inputs() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		isFocused = false;
		firstClick = true;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isFocused)
	{
		isFocused = true;
		firstClick = false;
	}
	if (isFocused && !firstClick)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		if (glfwRawMouseMotionSupported()) {
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
	}
	if (isFocused && firstClick)
	{
		firstClick = false;
	}
	if (!isFocused)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}
}

void Window::UpdateWindowSize(int newWindowWidth, int newWindowHeight) {
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;
}

int Window::GetWidth() {
	return windowWidth;
}

int Window::GetHeight() {
	return windowHeight;
}

const char* Window::GetTitle() {
	return windowTitle;
}

void Window::SetTitle(const char* newTitle) {
	glfwSetWindowTitle(window, newTitle);
}

GLFWwindow* Window::GetWindowInstance() {
	return window;
}

void Window::Delete() {
	glfwDestroyWindow(window);
	glfwTerminate();
}