#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Camera.h>


class Window {
	private:
		GLFWwindow* window;
		Camera* camera/* = Camera(600, 600, glm::vec3(0, 0, 0))*/;

	public:
		Window(int windowWidth, int windowHeight, const char* windowTitle, Camera* camera, GLFWframebuffersizefun callback);
		~Window();

		int GetWidth();
		int GetHeight();
		const char* GetTitle();
		GLFWwindow* GetWindowInstance();

		int windowWidth, windowHeight;
		const char* windowTitle;

};