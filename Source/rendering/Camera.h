#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Input.h"
#include "Shader.h"
#include "Window.h"

class Camera
{
	public:
		glm::mat4 viewMatrix = glm::mat4(1.0f);
		glm::mat4 projectionMatrix = glm::mat4(1.0f);

		Camera() : window(window), windowInstance(nullptr) {}
		Camera(Window& newWindow):  window(newWindow), windowInstance(nullptr) {};
		~Camera() {};

		void Setup(Window& window);

		void SetCameraMatrix(Shader& shader) const;
		void UpdateMatrix(float FOV, float nearPlane, float farPlane, glm::vec3 position, glm::vec3 orientation, glm::vec3 upDirection);

		Window& GetWindow();

	private:
		Window& window;
		GLFWwindow* windowInstance;
};