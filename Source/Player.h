#pragma once

#include <glad/glad.h>

#include <Camera.h>

class Player {
public:
	Player(int playerX, int playerY, int playerZ);
	~Player();

	void UpdatePosition(int playerX, int playerY, int playerZ);
	void SetCameraMatrix(Shader& shader, const char* uniform);
	void UpdateWindowSize(int windowWidth, int windowHeight);
	void Update(GLFWwindow* windowInstance, Window window, Shader& shader, const char* uniform, int newPlayerX, int newPlayerY, int newPlayerZ);

	std::tuple<int, int, int> GetPosition();

private:
	glm::vec3 playerPosition = glm::vec3(0, 0, 0);
	Camera camera = Camera(600, 600, playerPosition);
};