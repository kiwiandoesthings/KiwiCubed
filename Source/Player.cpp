#include <Player.h>


Player::Player(int playerX, int playerY, int playerZ) {
	camera.SetPosition((float)playerX, (float)playerY, (float)playerZ);
}

void Player::UpdatePosition(int newPlayerX, int newPlayerY, int newPlayerZ) {
	playerPosition = glm::vec3(newPlayerX, newPlayerY, newPlayerZ);
	camera.UpdateMatrix(80.0f, 0.1f, 1000.0f);
}

void Player::SetCameraMatrix(Shader& shader, const char* uniform) {
	camera.Matrix(shader, uniform);
}

void Player::UpdateWindowSize(int windowWidth, int windowHeight) {
	camera.UpdateWindowSize(windowWidth, windowHeight);
}

void Player::Update(GLFWwindow* windowInstance, Window window, Shader& shader, const char* uniform, int newPlayerX, int newPlayerY, int newPlayerZ) {
	camera.Inputs(windowInstance, window);
	UpdatePosition(newPlayerX, newPlayerY, newPlayerZ);
	SetCameraMatrix(shader, uniform);
}

std::tuple<int, int, int> Player::GetPosition() {
	return std::make_tuple(playerPosition.x, playerPosition.y, playerPosition.z);
}

Player::~Player() {

}