#include "Input.h"

InputHandler::InputHandler(Window* window) : window(window) {
}

bool InputHandler::GetKeyPressed(Key key) {
	GLFWwindow* windowInstance = window->GetWindowInstance();

	if (window->isFocused) {
		if (glfwGetKey(windowInstance, key) == GLFW_PRESS)
		{
			return true;
		}
	}
	return false;
}

InputHandler::~InputHandler() {

}