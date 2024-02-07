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

		void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
			glViewport(0, 0, width, height);
			float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
			glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
			//camera.UpdateWindowSize(width, height);
		}

	public:
		Window(int windowWidth, int windowHeight, const char* windowTitle);
		~Window();

		int GetWidth();
		int GetHeight();
		const char* GetTitle();
		GLFWwindow* GetWindowInstance();

		int windowWidth, windowHeight;
		const char* windowTitle;
};