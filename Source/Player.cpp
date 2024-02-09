#include <Player.h>


Player::Player(int playerX, int playerY, int playerZ) {
	Player::playerX = playerX;
	Player::playerY = playerY;
	Player::playerZ = playerZ;

	camera.SetPosition(playerX, playerY, playerZ);
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

void Player::Update(GLFWwindow* window, Shader& shader, const char* uniform, int newPlayerX, int newPlayerY, int newPlayerZ) {
	camera.Inputs(window);
	UpdatePosition(newPlayerX, newPlayerY, newPlayerZ);
	SetCameraMatrix(shader, uniform);
}

Camera Player::GetCameraInstance() {
	return camera;
}

Player::~Player() {

}