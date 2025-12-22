#include "Window.h"
#include "GLFW/glfw3.h"
#include <cstdio>
#include "stb_image.h"


Window::Window() = default;
Window::~Window() = default;

Window& Window::GetInstance() {
    static Window instance;
    return instance;
}

GLFWwindow* Window::CreateGLFWWindow(int windowWidth, int windowHeight, const char* windowTitle, const char* windowType, GLFWCallbacks callbacks) {
	OVERRIDE_LOG_NAME("Window");
	Window::windowWidth = windowWidth;
	Window::windowHeight = windowHeight;
	Window::windowTitle = windowTitle;

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	if (strcmp(windowType, "fullscreen") == 0) {
		glfwWindow = glfwCreateWindow(mode->width, mode->height, windowTitle, monitor, nullptr);
	}
	else if (strcmp(windowType, "window_borderless") == 0) {
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindow = glfwCreateWindow(mode->width, mode->height, windowTitle, nullptr, nullptr);
	}
	else if (strcmp(windowType, "window_maximized") == 0) {
		glfwWindow = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
		glfwMaximizeWindow(glfwWindow);
	}
	else if (strcmp(windowType, "window") == 0) {
		glfwWindow = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GL_CONTEXT_PROFILE_MASK, 24);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetFramebufferSizeCallback(glfwWindow, callbacks.framebufferSize);
	glfwSetWindowFocusCallback(glfwWindow, callbacks.windowFocus);

	if (!glfwWindow) {
		CRITICAL("Failed to create GLFW window");
		glfwTerminate();
	} else {
		INFO("Successfully created GLFW window");
	}

	int newWindowWidth;
	int newWindowHeight;

	glfwGetWindowSize(glfwWindow, &newWindowWidth, &newWindowHeight);

	GLFWimage processIcon;
	processIcon.pixels = stbi_load("Assets/kiwi_exe_64x.png", &processIcon.width, &processIcon.height, 0, 4);
	if (!processIcon.pixels) {
		CRITICAL("Failed to set process icon. Icon was likely not available at \"Assets/kiwi_exe_64x.png\"");
		psnip_trap();
	}

	glfwSetWindowIcon(glfwWindow, 1, &processIcon);
	stbi_image_free(processIcon.pixels);
	
	Window::windowWidth = newWindowWidth;
	Window::windowHeight = newWindowHeight;
	
	glfwMakeContextCurrent(glfwWindow);
	glfwSwapInterval(1);

	return glfwWindow;
}

void Window::Setup() {
	inputHandler.SetupCallbacks(glfwWindow);
}

void Window::QueryInputs() {
	if (!isFocused) {
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		cursorLocked = false;
		return;
	}
	if (isFocused) {
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		glfwSetInputMode(glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_CURSOR_HIDDEN);
		cursorLocked = true;
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
	glfwSetWindowTitle(glfwWindow, newTitle);
	windowTitle = newTitle;
}

void Window::SetTitle(std::string newTitle) {
	glfwSetWindowTitle(glfwWindow, newTitle.c_str());
	windowTitle = newTitle.c_str();
}

void Window::SetFocused(bool focus) {
	isFocused = focus;

	if (isFocused) {
		glfwSetCursorPos(glfwWindow, static_cast<float>(windowWidth) / 2, static_cast<float>(windowHeight) / 2);
	}
}

bool Window::GetFocused() const {
	return isFocused;
}

GLFWwindow* Window::GetGLFWWindow() {
	return glfwWindow;
}

void Window::Delete() {
	OVERRIDE_LOG_NAME("Window");
	glfwDestroyWindow(glfwWindow);
	glfwTerminate();
	INFO("Terminated window and GLFW");
}