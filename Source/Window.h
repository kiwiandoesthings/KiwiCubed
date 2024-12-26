#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "Input.h"


class Window {
	public:
		Window() {};
		
		GLFWwindow* CreateWindowInstance(int windowWidth, int windowHeight, const char* windowTitle, const char* windowType);

		void Setup();

		void QueryInputs();

		void UpdateWindowSize(int newWindowWidth, int newWindowHeight);

		int GetWidth() const;
		int GetHeight() const;
		const char* GetTitle() const;
		void SetTitle(const char* newTitle);
		void SetTitle(std::string newTitle);

		GLFWwindow* GetWindowInstance();

		void Delete();

		int windowWidth, windowHeight;
		const char* windowTitle;

		bool isFocused;

	private:
		GLFWwindow* window;
		InputHandler inputHandler = InputHandler();
};