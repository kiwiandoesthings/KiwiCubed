#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Window {
	private:
		GLFWwindow* window;

	public:
		Window(int windowWidth, int windowHeight, const char* windowTitle, GLFWframebuffersizefun callback);
		~Window();

		int GetWidth();
		int GetHeight();
		const char* GetTitle();
		GLFWwindow* GetWindowInstance();

		int windowWidth, windowHeight;
		const char* windowTitle;
};