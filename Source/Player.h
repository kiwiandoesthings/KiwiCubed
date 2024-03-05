#pragma once

#include <GLError.h>
#include <glad/glad.h>

#include <Camera.h>

class Player {
public:
	Player(int playerX, int playerY, int playerZ);

	void Update(Window* window, Shader& shader, const char* uniform, int newPlayerX, int newPlayerY, int newPlayerZ);
	void UpdatePosition(Window* window, int playerX, int playerY, int playerZ);
	void SetCameraMatrix(Shader& shader, const char* uniform);

	std::tuple<int, int, int> GetPosition();

	void Delete();

private:
	glm::vec3 playerPosition = glm::vec3(0, 0, 0);
	Camera camera = Camera(600, 600, playerPosition);
};