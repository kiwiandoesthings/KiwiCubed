#include "Player.h"
#include "Entity.h"
#include "Events.h"
#include "World/ChunkHandler.h"
#include <klogger.hpp>

const char* gameModeStrings[] = {
	"Survival",
	"Creative"
};


Player::Player(int playerX, int playerY, int playerZ, ChunkHandler& chunkHandler) : Entity(), yaw(0), pitch(0), roll(0), width(640), height(480), chunkHandler(chunkHandler) {
	entityData.position = glm::vec3(playerX, playerY, playerZ);
	
	entityStats.health = 20.0f;
	entityStats.armor = 0;
	
	entityData.name = "Player";
	
	entityData.physicsBoundingBox = PhysicsBoundingBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f));

	inputHandler.SetupCallbacks(Window::GetInstance().GetWindowInstance());
	inputCallbackIDs.emplace_back(inputHandler.RegisterMouseButtonCallback(GLFW_MOUSE_BUTTON_LEFT, std::bind(&Player::MouseButtonCallback, this, std::placeholders::_1)));
	inputCallbackIDs.emplace_back(inputHandler.RegisterMouseButtonCallback(GLFW_MOUSE_BUTTON_RIGHT, std::bind(&Player::MouseButtonCallback, this, std::placeholders::_1)));

	playerData.gameMode = SURVIVAL;

	if (playerData.gameMode == CREATIVE) {
		entityData.applyCollision = false;
		entityData.applyGravity = false;
	} else {
		entityData.applyCollision = true;
		entityData.applyGravity = true;
	}
}

void Player::Setup() {
	camera = std::make_shared<Camera>();

	inputCallbackIDs.emplace_back(inputHandler.RegisterKeyCallback(GLFW_KEY_F4, [&](int key) {
		if (playerData.gameMode == SURVIVAL) {
			playerData.gameMode = CREATIVE;
			entityData.applyGravity = false;
			entityData.applyCollision = false;
		} else {
			playerData.gameMode = SURVIVAL;
			entityData.applyGravity = true;
			entityData.applyCollision = true;
		}
	}));
	inputCallbackIDs.emplace_back(inputHandler.RegisterKeyCallback(GLFW_KEY_F3, [&](int key) {
		EventManager::GetInstance().TriggerEvent("event/toggle_debug");
	}));
	inputCallbackIDs.emplace_back(inputHandler.RegisterKeyCallback(GLFW_KEY_ESCAPE, [&](int key) {
		UI& ui = UI::GetInstance();
		EventManager& eventManager = EventManager::GetInstance();
		if (ui.IsDisabled()) {
			eventManager.TriggerEvent("ui/move_screen_game_pause");
			Window::GetInstance().SetFocused(false);
		} else {
			eventManager.TriggerEvent("event/back_ui");
		}
	}));
	inputCallbackIDs.emplace_back(inputHandler.RegisterScrollCallback(true, [this](double offset) {
		entityStats.health += static_cast<float>(offset);
	}));
}

void Player::Update() {
	OVERRIDE_LOG_NAME("Player Update");
	if (!camera) {
		WARN("Trying to update player without a camera, aborting");
		return;
	}

	if (camera->GetWindow().GetFocused()) {
		EntityData oldEntityData = entityData;
		QueryInputs();
		QueryMouseInputs();
		entityData.isGrounded = GetGrounded(*this, chunkHandler);
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
		glm::vec3 up = entityData.upDirection;
		up.x = 0;
		up.z = 0;

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
		if (inputHandler.GetKeyState(GLFW_KEY_SPACE) && entityData.isGrounded) {
			std::cout << "jump" << std::endl;
			movementVector += up;
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
	if (!window.GetFocused()) {
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

glm::vec3 Player::GetPosition() const {
	return glm::vec3(entityData.position.x, entityData.position.y, entityData.position.z);
}

GameMode Player::GetGameMode() const {
	return playerData.gameMode;
}

std::string Player::GetGameModeString() const {
	return gameModeStrings[playerData.gameMode];
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
	for (unsigned int id : inputCallbackIDs) {
		inputHandler.DeregisterCallback(id, "Player");
	}
	inputCallbackIDs.clear();
}