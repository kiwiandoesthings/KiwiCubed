#include "Player.h"


static int positiveModulo(int a, int b) {
	return (a % b + b) % b;
}


Player::Player(int playerX, int playerY, int playerZ) : width(640), height(480), yaw(0), pitch(0), roll(0), Entity() {
	entityData.position = glm::vec3(playerX, playerY, playerZ);
	
	entityStats.health = 20.0f;
	entityStats.armor = 0;
	
	entityData.name = "Player";
	
	entityData.physicsBoundingBox = PhysicsBoundingBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f));
}

void Player::Setup(Window& window) {
	//Player::window = window;
	camera = std::make_shared<Camera>(window);
	camera->Setup(window);
	inputHandler.SetupKeyStates(window.GetWindowInstance(), std::vector<int>{GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_LEFT_CONTROL});
}

void Player::Update(Window* window, ChunkHandler& chunkHandler) {
	EntityData oldData = entityData;
	if (!camera) {
		std::cerr << "[Player Update / Warn] Trying to update player without a camera, aborting" << std::endl;
		return;
	}

	if (camera->GetWindow().isFocused) {
		QueryInputs(chunkHandler);
		QueryMouseInputs();
		ApplyPhysics(*this, chunkHandler);
	}

	//entityData.velocity.x = entityData.position.x - oldData.position.x;
	//entityData.velocity.y = entityData.position.y - oldData.position.y;
	//entityData.velocity.z = entityData.position.z - oldData.position.z;
}

void Player::UpdateShader(Shader& shader, const char* uniform) {
	UpdateCameraMatrix(shader, uniform);
}

void Player::QueryInputs(ChunkHandler& chunkHandler) {
	Window& window = camera->GetWindow();
	if (inputHandler.GetKeyState(GLFW_KEY_W)) {
		//entityData.position += speed * entityData.orientation;
		entityData.velocity += speed * entityData.orientation;
	}
	if (inputHandler.GetKeyState(GLFW_KEY_A)) {
		//entityData.position += speed * -glm::normalize(glm::cross(entityData.orientation, entityData.upDirection));
		entityData.velocity += speed * -glm::normalize(glm::cross(entityData.orientation, entityData.upDirection));
	}
	if (inputHandler.GetKeyState(GLFW_KEY_S)) {
		//entityData.position += speed * -entityData.orientation;
		entityData.velocity += speed * -entityData.orientation;
	}
	if (inputHandler.GetKeyState(GLFW_KEY_D)) {
		//entityData.position += speed * glm::normalize(glm::cross(entityData.orientation, entityData.upDirection));
		entityData.velocity += speed * glm::normalize(glm::cross(entityData.orientation, entityData.upDirection));
	}
	if (inputHandler.GetKeyState(GLFW_KEY_SPACE)) {
		//entityData.position += speed * entityData.upDirection;
		entityData.velocity += speed * entityData.upDirection;
	}
	if (inputHandler.GetKeyState(GLFW_KEY_LEFT_SHIFT)) {
		//entityData.position += speed * -entityData.upDirection;
		entityData.velocity += speed * -entityData.upDirection;
	}
	if (inputHandler.GetKeyState(GLFW_KEY_LEFT_CONTROL)) {
		speed = .8f;
	}
	else {
		speed = 0.01f;
	}
}

void Player::QueryMouseInputs() {
	Window& window = camera->GetWindow();
	if (!window.isFocused) {
		return;
	}

	inputHandler.RegisterScrollCallback(true, [this](double offset) {
		entityStats.health += static_cast<float>(offset);
		});

	if (inputHandler.GetKeyState(GLFW_KEY_MINUS)) {
		entityStats.health -= 0.1f;
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
	entityData.orientation = glm::normalize(facing);

	// We don't want anyone to be able to move the mouse off the screen, that would be very very very bad and horrible and would make the game absolutely unplayable
	glfwSetCursorPos(window.GetWindowInstance(), (static_cast<float>(window.GetWidth() / 2)), (static_cast<float>(window.GetHeight() / 2)));
}

void Player::SetPosition(Window* window, int newPlayerX, int newPlayerY, int newPlayerZ) {
	if (!camera) {
		std::cerr << "[Player Update / Warn] Trying to update player without a camera, aborting" << std::endl;
		return;
	}
	entityData.position = glm::vec3(newPlayerX, newPlayerY, newPlayerZ);
	camera->UpdateMatrix(80.0f, 0.1f, 1000.0f, entityData.position, entityData.orientation, entityData.upDirection);
}

const std::tuple<int, int, int> Player::GetPosition() {
	return std::make_tuple(static_cast<int>(entityData.position.x), static_cast<int>(entityData.position.y), static_cast<int>(entityData.position.z));
}

void Player::UpdateCameraMatrix(Shader& shader, const char* uniform) {
	if (!camera) {
		std::cerr << "[Camera Matrix / Warn] Trying to update camera matrix without a camera, aborting" << std::endl;
	}
	camera->UpdateMatrix(80.0f, 0.1f, 1000.0f, entityData.position, entityData.orientation, entityData.upDirection);
	camera->SetCameraMatrix(shader, uniform);
}

void Player::Delete() {

}