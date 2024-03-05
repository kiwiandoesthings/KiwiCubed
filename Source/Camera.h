#pragma once

#include <GLError.h>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <Shader.h>
#include <Window.h>

class Camera
{
	public:
		glm::vec3 position;
		glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 cameraMatrix = glm::mat4(1.0f);

		int width;
		int height;

		float speed = 0.004f;
		float sensitivity = 100.0f;

		Camera(int width, int height, glm::vec3 position);

		void Matrix(Shader& shader, const char* uniform);
		void UpdateMatrix(Window* window, float FOV, float nearPlane, float farPlane);
		void SetPosition(float cameraX, float cameraY, float cameraZ);
		void Inputs(Window* window);
};