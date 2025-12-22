#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <debug-trap.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>

#include "Input.h"


struct GLFWCallbacks {
	GLFWframebuffersizefun framebufferSize;
	GLFWwindowfocusfun windowFocus;
};


class Window {
	public:
		static Window& GetInstance();

		GLFWwindow* CreateGLFWWindow(int windowWidth, int windowHeight, const char* windowTitle, const char* windowType, GLFWCallbacks callbacks);

		void Setup();

		void QueryInputs();

		void UpdateWindowSize(int newWindowWidth, int newWindowHeight);

		int GetWidth() const;
		int GetHeight() const;

		const char* GetTitle() const;

		void SetTitle(const char* newTitle);
		void SetTitle(std::string newTitle);

		void SetFocused(bool focus);
		bool GetFocused() const;

		GLFWwindow* GetGLFWWindow();

		void Delete();

		int windowWidth, windowHeight;
		const char* windowTitle;

		
		private:
		Window();
        ~Window();
		
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
		
		GLFWwindow* glfwWindow;
		InputHandler inputHandler = InputHandler("Window");

		bool isFocused;
		bool cursorLocked;
};