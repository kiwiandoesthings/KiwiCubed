#include "Player.h"


Player::Player(int playerX, int playerY, int playerZ) : width(640), height(480), yaw(0), pitch(0), roll(0) {
	playerPosition = glm::vec3(playerX, playerY, playerZ);
}

void Player::Setup(Window& window) {
	//Player::window = window;
	camera = std::make_shared<Camera>(window);
	camera->Setup(window);
	inputHandler.SetupKeyStates(window.GetWindowInstance(), std::vector<int>{GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_LEFT_CONTROL});
}

void Player::Update(Window* window, Shader& shader, const char* uniform) {
	if (!camera) {
		std::cerr << "[Player Update / Warn] Trying to update player without a camera, aborting" << std::endl;
		return;
	}
	QueryInputs();
	UpdateCameraMatrix(shader, uniform);
}

void Player::QueryInputs() {
	Window& window = camera->GetWindow();
	if (!window.isFocused) {
		return;
	}
	if (inputHandler.GetKeyState(GLFW_KEY_W)) {
		playerPosition += speed * orientation;
	}
	if (inputHandler.GetKeyState(GLFW_KEY_A)) {
		playerPosition += speed * -glm::normalize(glm::cross(orientation, upDirection));
	}
	if (inputHandler.GetKeyState(GLFW_KEY_S)) {
		playerPosition += speed * -orientation;
	}
	if (inputHandler.GetKeyState(GLFW_KEY_D)) {
		playerPosition += speed * glm::normalize(glm::cross(orientation, upDirection));
	}
	if (inputHandler.GetKeyState(GLFW_KEY_SPACE)) {
		playerPosition += speed * upDirection;
	}
	if (inputHandler.GetKeyState(GLFW_KEY_LEFT_SHIFT)) {
		playerPosition += speed * -upDirection;
	}
	if (inputHandler.GetKeyState(GLFW_KEY_LEFT_CONTROL)) {
		speed = .8f;
	} else {
		speed = 0.01f;
	}

	// Does some absolute magic to rotate the camera correctly
	double mouseX;
	double mouseY;

	glfwGetCursorPos(window.GetWindowInstance(), &mouseX, &mouseY);

	// Get the amount to rotate for the frame
	float rotationX = sensitivity * static_cast<float>(mouseY - (window.GetHeight() / 2)) / window.GetHeight();
	float rotationY = sensitivity * static_cast<float>(mouseX - (window.GetWidth() / 2)) / window.GetWidth();

	yaw += rotationY;
	pitch += rotationX;

	// Clamp pitch to prevent the camera from flipping out
	if (pitch > 89.9f)
		pitch = 89.9f;
	if (pitch < -89.9f)
		pitch = -89.9f;

	// wha..? (learnopengl.com)
	glm::vec3 facing = glm::vec3(0, 0, 0);
	facing.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	facing.y = sin(glm::radians(-pitch));
	facing.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	orientation = glm::normalize(facing);

	// We don't want anyone to be able to move the mouse off the screen, that would be very very very bad and horrible and would make the game absolutely unplayable
	glfwSetCursorPos(window.GetWindowInstance(), (static_cast<float>(window.GetWidth() / 2)), (static_cast<float>(window.GetHeight() / 2)));
}

void Player::SetPosition(Window* window, int newPlayerX, int newPlayerY, int newPlayerZ) {
	if (!camera) {
		std::cerr << "[Player Update / Warn] Trying to update player without a camera, aborting" << std::endl;
		return;
	}
	playerPosition = glm::vec3(newPlayerX, newPlayerY, newPlayerZ);
	camera->UpdateMatrix(80.0f, 0.1f, 1000.0f, playerPosition, orientation, upDirection);
}

const std::tuple<int, int, int> Player::GetPosition() {
	return std::make_tuple(static_cast<int>(playerPosition.x), static_cast<int>(playerPosition.y), static_cast<int>(playerPosition.z));
}

void Player::UpdateCameraMatrix(Shader& shader, const char* uniform) {
	if (!camera) {
		std::cerr << "[Camera Matrix / Warn] Trying to update camera matrix without a camera, aborting" << std::endl;
	}
	camera->UpdateMatrix(80.0f, 0.1f, 1000.0f, playerPosition, orientation, upDirection);
	camera->Matrix(shader, uniform);
}

void Player::Delete() {

}