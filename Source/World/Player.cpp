#include "Player.h"
#include "World/ChunkHandler.h"
#include <klogger.hpp>


Player::Player(int playerX, int playerY, int playerZ, ChunkHandler& chunkHandler) : Entity(), yaw(0), pitch(0), roll(0), width(640), height(480), chunkHandler(chunkHandler) {
	entityData.position = glm::vec3(playerX, playerY, playerZ);
	
	entityStats.health = 20.0f;
	entityStats.armor = 0;
	
	entityData.name = "Player";
	
	entityData.physicsBoundingBox = PhysicsBoundingBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f));

	inputHandler.SetupCallbacks(Window::GetInstance().GetWindowInstance());
	inputHandler.RegisterMouseButtonCallback(GLFW_MOUSE_BUTTON_LEFT, std::bind(&Player::MouseButtonCallback, this, std::placeholders::_1));
	inputHandler.RegisterMouseButtonCallback(GLFW_MOUSE_BUTTON_RIGHT, std::bind(&Player::MouseButtonCallback, this, std::placeholders::_1));
}

void Player::Setup() {
	camera = std::make_shared<Camera>();

	inputHandler.RegisterKeyCallback(GLFW_KEY_E, [&](int key) {
		DEBUG("clicked key e");
		EventManager::GetInstance().TriggerEvent("event/unload_world");
	});
	inputHandler.RegisterKeyCallback(GLFW_KEY_R, [&](int key) {
		DEBUG("clicked key r");
		EventManager::GetInstance().TriggerEvent("event/generate_world");
	});
	inputHandler.RegisterKeyCallback(GLFW_KEY_F4, [&](int key) {
		if (playerData.gameMode == SURVIVAL) {
			playerData.gameMode = CREATIVE;
			entityData.applyGravity = false;
			entityData.applyCollision = false;
		} else {
			playerData.gameMode = SURVIVAL;
			entityData.applyGravity = true;
			entityData.applyCollision = true;
		}
	});
	inputHandler.RegisterScrollCallback(true, [this](double offset) {
		entityStats.health += static_cast<float>(offset);
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
			EventManager::GetInstance().TriggerEvent("event/entity_moved_chunk", std::make_pair("globalChunkPosition", entityData.globalChunkPosition));
		}
	}
}

void Player::QueryInputs() {
	glm::vec3 movementVector = glm::vec3(0);

	if (inputHandler.GetKeyState(GLFW_KEY_LEFT_CONTROL)) {
		speed = .5f;
	} else {
		speed = 0.1f;
	}

	if (playerData.gameMode == CREATIVE) {
		glm::vec3 forward = entityData.orientation;
		glm::vec3 right = glm::normalize(glm::cross(entityData.orientation, entityData.upDirection));
		glm::vec3 up = entityData.upDirection;

		if (inputHandler.GetKeyState(GLFW_KEY_W)) {
			movementVector += forward;
		}
		if (inputHandler.GetKeyState(GLFW_KEY_A)) {
			movementVector += -right;
		}
		if (inputHandler.GetKeyState(GLFW_KEY_S)) {
			movementVector += -forward;
		}
		if (inputHandler.GetKeyState(GLFW_KEY_D)) {
			movementVector += right;
		}
		if (inputHandler.GetKeyState(GLFW_KEY_SPACE)) {
			movementVector += up;
		}
		if (inputHandler.GetKeyState(GLFW_KEY_LEFT_SHIFT)) {
			movementVector += -up;
		}
	} else {
		glm::vec3 forward = entityData.orientation;
		forward.y = 0;
		forward = glm::normalize(forward);
		glm::vec3 right = glm::normalize(glm::cross(forward, entityData.upDirection));

		if (inputHandler.GetKeyState(GLFW_KEY_W)) {
			movementVector += forward;
		}
		if (inputHandler.GetKeyState(GLFW_KEY_A)) {
			movementVector += -right;
		}
		if (inputHandler.GetKeyState(GLFW_KEY_S)) {
			movementVector += -forward;
		}
		if (inputHandler.GetKeyState(GLFW_KEY_D)) {
			movementVector += right;
		}
	}

	if (glm::length(movementVector) > 0.0f) {
		movementVector = glm::normalize(movementVector) * speed;
	}

	entityData.velocity = movementVector;
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