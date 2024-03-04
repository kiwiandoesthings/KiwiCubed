#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLError.h>


class Window {
	private:
		GLFWwindow* window;
		bool firstClick;

	public:
		Window() : windowWidth(640), windowHeight(480), windowTitle("Failed to retrieve window title"), isFocused(false), firstClick(true) {}
		Window(int windowWidth, int windowHeight, const char* windowTitle);
		~Window();

		void Inputs();

		void UpdateWindowSize(int windowWidth, int windowHeight);
		int GetWidth();
		int GetHeight();
		const char* GetTitle();
		GLFWwindow* GetWindowInstance();

		int windowWidth, windowHeight;
		const char* windowTitle;

		bool isFocused;
};