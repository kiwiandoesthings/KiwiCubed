#pragma once

#include "GLError.h"
#include <GLAD/glad.h>

#include "Camera.h"

class Player {
public:
	Player(int playerX, int playerY, int playerZ);

	void Update(Window* window, Shader& shader, const char* uniform, int newPlayerX, int newPlayerY, int newPlayerZ);

	void SetPosition(Window* window, int playerX, int playerY, int playerZ);
	std::tuple<int, int, int> GetPosition();

	void SetCameraMatrix(Shader& shader, const char* uniform);


	void Delete();

private:
	glm::vec3 playerPosition = glm::vec3(0, 0, 0);
	Camera camera = Camera(600, 600, playerPosition);
};