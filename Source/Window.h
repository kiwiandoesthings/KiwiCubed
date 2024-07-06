#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <GLError.h>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <thread>

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include <GLError.h>


class Window {
	private:
		GLFWwindow* window;
		bool firstClick;

	public:
		Window() : windowWidth(640), windowHeight(480), windowTitle("Failed to retrieve window title"), isFocused(false), firstClick(true), window(window) {}
		Window(int windowWidth, int windowHeight, const char* windowTitle);

		void Inputs();

		void UpdateWindowSize(int newWindowWidth, int newWindowHeight);

		int GetWidth();

		int GetHeight();

		const char* GetTitle();
		void SetTitle(const char* newTitle);

		GLFWwindow* GetWindowInstance();

		void Delete();

		int windowWidth, windowHeight;
		const char* windowTitle;

		bool isFocused;
};