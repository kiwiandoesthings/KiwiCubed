#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "GLError.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "Input.h"


class Window {
	public:
		Window() : windowWidth(640), windowHeight(480), windowTitle(""), isFocused(false), window(window) {}
		Window(int windowWidth, int windowHeight, const char* windowTitle, const char* windowType);
		Window(int windowWidth, int windowHeight, std::string windowTitle, std::string windowType);

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