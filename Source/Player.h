#pragma once

#include "GLError.h"
#include <GLAD/glad.h>

#include "Camera.h"

class Player {
	public:
		glm::vec3 orientation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, 0.0f);

		float yaw = -90.0f;
		float pitch;
		float roll;

		int width;
		int height;

		float speed = 0.004f;
		float sensitivity = 100.0f;

		Player(int playerX, int playerY, int playerZ);
	
		void Setup(Window& window);
	
		void Update(Window* window, Shader& shader, const char* uniform);
		void QueryInputs();
	
		void SetPosition(Window* window, int playerX, int playerY, int playerZ);
		const std::tuple<int, int, int> GetPosition();
	
		void UpdateCameraMatrix(Shader& shader, const char* uniform);
	
		void Delete();
	
	private:
		glm::vec3 playerPosition = glm::vec3(0, 0, 0);
		std::shared_ptr<Camera> camera;
		InputHandler inputHandler = InputHandler();
};