#include "Player.h"


Player::Player(int playerX, int playerY, int playerZ) {
	camera.SetPosition((float)playerX, (float)playerY, (float)playerZ);
}

void Player::Update(Window* window, Shader& shader, const char* uniform, int newPlayerX, int newPlayerY, int newPlayerZ) {
	camera.Inputs(window);
	SetPosition(window, newPlayerX, newPlayerY, newPlayerZ);
	SetCameraMatrix(shader, uniform);
}

void Player::SetPosition(Window* window, int newPlayerX, int newPlayerY, int newPlayerZ) {
	playerPosition = glm::vec3(newPlayerX, newPlayerY, newPlayerZ);
	camera.UpdateMatrix(window, 80.0f, 0.1f, 1000.0f);
}

void Player::SetCameraMatrix(Shader& shader, const char* uniform) {
	camera.Matrix(shader, uniform);
}

std::tuple<int, int, int> Player::GetPosition() {
	return std::make_tuple((int)playerPosition.x, (int)playerPosition.y, (int)playerPosition.z);
}

void Player::Delete() {

}