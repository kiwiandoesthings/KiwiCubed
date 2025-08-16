#include "Player.h"
#include "World/ChunkHandler.h"
#include <klogger.hpp>


Player::Player(int playerX, int playerY, int playerZ, ChunkHandler& chunkHandler) : Entity(), yaw(0), pitch(0), roll(0), width(640), height(480), chunkHandler(chunkHandler) {
	entityData.position = glm::vec3(playerX, playerY, playerZ);
	
	entityStats.health = 20.0f;
	entityStats.armor = 0;
	
	entityData.name = "Player";
	
	entityData.physicsBoundingBox = PhysicsBoundingBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f));

	inputHandler.RegisterMouseButtonCallback(GLFW_MOUSE_BUTTON_LEFT, std::bind(&Player::MouseButtonCallback, this, std::placeholders::_1));
	inputHandler.RegisterMouseButtonCallback(GLFW_MOUSE_BUTTON_RIGHT, std::bind(&Player::MouseButtonCallback, this, std::placeholders::_1));
}

void Player::Setup(Window& window) {
	camera = std::make_shared<Camera>(window);
	camera->Setup(window);
	inputHandler.SetupKeyStates(window.GetWindowInstance(), std::vector<int>{GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_LEFT_CONTROL});

	inputHandler.RegisterKeyCallback(GLFW_KEY_E, [&](int key) {
		chunkHandler.Delete();
	});
	inputHandler.RegisterKeyCallback(GLFW_KEY_R, [&](int key) {
		chunkHandler.GenerateWorld();
	});
}

void Player::Update() {
	OVERRIDE_LOG_NAME("Player Update");
	if (!camera) {
		WARN("Trying to update player without a camera, aborting");
		return;
	}

	if (camera->GetWindow().isFocused) {
		EntityData oldEntityData = entityData;
		QueryInputs();
		QueryMouseInputs();
		ApplyPhysics(*this, chunkHandler, entityData.applyGravity, entityData.applyCollision);

		if (oldEntityData.globalChunkPosition != entityData.globalChunkPosition) {
			EventManager::GetInstance().TriggerEvent("EntityMovedChunk", make_pair("globalChunkPosition", std::any(entityData.globalChunkPosition)));
		}
	}
}

void Player::QueryInputs() {
	if (inputHandler.GetKeyState(GLFW_KEY_W)) {
		entityData.velocity += speed * entityData.orientation;
	}
	if (inputHandler.GetKeyState(GLFW_KEY_A)) {
		entityData.velocity += speed * -glm::normalize(glm::cross(entityData.orientation, entityData.upDirection));
	}
	if (inputHandler.GetKeyState(GLFW_KEY_S)) {
		entityData.velocity += speed * -entityData.orientation;
	}
	if (inputHandler.GetKeyState(GLFW_KEY_D)) {
		entityData.velocity += speed * glm::normalize(glm::cross(entityData.orientation, entityData.upDirection));
	}
	if (inputHandler.GetKeyState(GLFW_KEY_SPACE)) {
		entityData.velocity += speed * entityData.upDirection;
	}
	if (inputHandler.GetKeyState(GLFW_KEY_LEFT_SHIFT)) {
		entityData.velocity += speed * -entityData.upDirection;
	}
	if (inputHandler.GetKeyState(GLFW_KEY_LEFT_CONTROL)) {
		speed = .8f;
	}
	else {
		speed = 0.01f;
	}
}

void Player::MouseButtonCallback(int button) {
	glm::ivec3 chunkPosition = glm::ivec3(0, 0, 0);
	glm::ivec3 blockPosition = glm::ivec3(0, 0, 0);
	bool hit = 0;
	if (RaycastWorld(entityData.position, entityData.orientation, 500, chunkHandler, blockPosition, chunkPosition, hit)) {
		if (button == 0) {
			chunkHandler.RemoveBlock(chunkPosition.x, chunkPosition.y, chunkPosition.z, blockPosition.x, blockPosition.y, blockPosition.z);
			if (blockPosition.x == 0 || blockPosition.x == chunkSize - 1 || blockPosition.y == 0 || blockPosition.y == chunkSize - 1 || blockPosition.z == 0 || blockPosition.z == chunkSize - 1) {
				chunkHandler.RemeshChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z, false);
				if (blockPosition.x == 0 || blockPosition.x == chunkSize - 1) {
					chunkHandler.RemeshChunk(chunkPosition.x - 1, chunkPosition.y, chunkPosition.z, false);
				}
				if (blockPosition.y == 0 || blockPosition.y == chunkSize - 1) {
					chunkHandler.RemeshChunk(chunkPosition.x, chunkPosition.y - 1, chunkPosition.z, false);
				}
				if (blockPosition.z == 0 || blockPosition.z == chunkSize - 1) {
					chunkHandler.RemeshChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z - 1, false);
				}
				if (blockPosition.x == chunkSize - 1) {
					chunkHandler.RemeshChunk(chunkPosition.x + 1, chunkPosition.y, chunkPosition.z, false);
				}
				if (blockPosition.y == chunkSize - 1) {
					chunkHandler.RemeshChunk(chunkPosition.x, chunkPosition.y + 1, chunkPosition.z, false);
				}
				if (blockPosition.z == chunkSize - 1) {
					chunkHandler.RemeshChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z + 1, false);
				}
			} else {
				chunkHandler.RemeshChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z, false);
			}
		}
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

	if (!(mouseX == oldMouseX && mouseY == oldMouseY)) {
		// Get the amount to rotate for the frame
		float rotationX = sensitivity * static_cast<float>(mouseY - (static_cast<float>(window.GetHeight()) / 2)) / window.GetHeight();
		float rotationY = sensitivity * static_cast<float>(mouseX - (static_cast<float>(window.GetWidth()) / 2)) / window.GetWidth();

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
	}

	// We don't want anyone to be able to move the mouse off the screen, that would be very very very bad and horrible and would make the game absolutely unplayable
	glfwSetCursorPos(window.GetWindowInstance(), (static_cast<float>(window.GetWidth()) / 2.0), (static_cast<float>(window.GetHeight()) / 2.0));

	oldMouseX = mouseX;
	oldMouseY = mouseY;
}

void Player::SetPosition(float newPlayerX, float newPlayerY, float newPlayerZ) {
	OVERRIDE_LOG_NAME("Player Update");
	if (!camera) {
		WARN("Trying to update player without a camera, aborting");
		return;
	}
	entityData.position = glm::vec3(newPlayerX, newPlayerY, newPlayerZ);
	camera->UpdateMatrix(80.0f, 0.1f, 1000.0f, entityData.position, entityData.orientation, entityData.upDirection);
}

const glm::vec3 Player::GetPosition() {
	return glm::vec3(entityData.position.x, entityData.position.y, entityData.position.z);
}

void Player::UpdateCameraMatrix(Shader& shader) {
	OVERRIDE_LOG_NAME("Camera Matrix");
	if (!camera) {
		WARN("Trying to update camera matrix without a camera, aborting");
	}
	camera->UpdateMatrix(80.0f, 0.1f, 1000.0f, entityData.position, entityData.orientation, entityData.upDirection);
	camera->SetCameraMatrix(shader);
}

void Player::Delete() {

}