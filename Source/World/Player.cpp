#include "Player.h"
#include "Block.h"
#include "ChunkHandler.h"
#include "Entity.h"
#include "Events.h"
#include "Physics.h"
#include "World.h"
#include <chrono>


const char* gameModeStrings[] = {
	"Survival",
	"Creative"
};


Player::Player(int playerX, int playerY, int playerZ, World* world) : Entity(static_cast<float>(playerX), static_cast<float>(playerY), static_cast<float>(playerZ), world), yaw(0), pitch(0), roll(0), width(640), height(480), world(world) {
	entityTransform.position = glm::vec3(playerX, playerY, playerZ);
	
	entityStats.health = 20.0f;
	entityStats.armor = 0;
	
	entityData.name = "Player";
	
	entityData.physicsBoundingBox = BoundingBox(glm::vec3(-0.3f, -1.62f, -0.3f), glm::vec3(0.3f, 0.18f, 0.3f));

	std::vector<AssetStringID> slotStringIDs;
	slotStringIDs.reserve(27);
	for (int slot = 0; slot < 27; slot++) {
		slotStringIDs.push_back(AssetStringID{"kiwicubed", "inventory_slot_" + fmt::format("{:02}", slot)});
	}

	entityData.inventory = Inventory(slotStringIDs);

	inputHandler.SetupCallbacks(Window::GetInstance().GetGLFWWindow());
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

	entityData.isPlayer = true;

	protectedEntityData.AUID = CreateAUID();

	chunkHandler = &world->GetChunkHandler();
}

void Player::Setup() {
	EventManager& eventManager = EventManager::GetInstance();
	camera = std::make_shared<Camera>();

	Window& globalWindow = Window::GetInstance();
	UIScreen inventoryUI = UIScreen("ui/inventory");
	int containerX = (globalWindow.GetWidth() / 2) - 576;
	int containerY = (globalWindow.GetHeight() / 2) - 192;
	inventoryUI.AddUIElement(std::make_unique<UIImage>(glm::vec2(containerX, containerY), glm::vec2(1152, 384), [&](){}, AssetStringID{"kiwicubed", "texture/inventory"}, AssetStringID{"kiwicubed", "ui_atlas"}).release());
	UI::GetInstance().AddScreen(std::move(inventoryUI));

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
		Globals::GetInstance().debugMode = true;
	}));
	inputCallbackIDs.emplace_back(inputHandler.RegisterKeyCallback(GLFW_KEY_ESCAPE, [&](int key) {
		UI& ui = UI::GetInstance();
		if (ui.IsDisabled()) {
			ui.SetCurrentScreen("ui/game_pause");
			Window::GetInstance().SetFocused(false);
		} else {
			UI::GetInstance().MoveScreenBack();
		}
	}));
	inputCallbackIDs.emplace_back(inputHandler.RegisterKeyCallback(GLFW_KEY_E, [&](int key) {
		if (!inInterface) {
			UI::GetInstance().SetCurrentScreen("ui/inventory");
		} else {
			UI::GetInstance().MoveScreenBack();
		}
		inInterface = !inInterface;
	}));
	inputCallbackIDs.emplace_back(inputHandler.RegisterScrollCallback(true, [this](double offset) {
		entityStats.health += static_cast<float>(offset);
	}));

	entityData.currentChunkPtr = chunkHandler->GetChunk(entityTransform.globalChunkPosition.x, entityTransform.globalChunkPosition.y, entityTransform.globalChunkPosition.z, false);
	if (!entityData.currentChunkPtr->IsReal()) {
		entityData.currentChunkPtr = nullptr;
	}
}

void Player::Update() {
	OVERRIDE_LOG_NAME("Player Update");
	if (!camera) {
		WARN("Trying to update player without a camera, aborting");
		return;
	}

	if (camera->GetWindow().GetFocused()) {
		EntityTransform oldEntityTransform = entityTransform;
		QueryInputs();
		QueryMouseInputs();
		Entity::Update();
		entityData.isGrounded = Physics::GetGrounded(*this, *chunkHandler);
		Physics::ApplyPhysics(*this, *chunkHandler, entityData.applyGravity, entityData.applyCollision);
        if (entityData.isGrounded) {
			if (entityData.isJumping) {
				//auto current = std::chrono::steady_clock::now();
				//std::cout << "jump took " << std::chrono::duration_cast<std::chrono::milliseconds>(current - jumpStart).count() << "ms" << std::endl; 
			}
			entityData.isJumping = false;
		}
		
		if (oldEntityTransform.globalChunkPosition != entityTransform.globalChunkPosition) {
			EventWorldPlayerMove moveEvent = EventWorldPlayerMove(oldEntityTransform.position.x, oldEntityTransform.position.y, oldEntityTransform.position.z, oldEntityTransform.orientation.y, oldEntityTransform.orientation.x, oldEntityTransform.orientation.z, entityTransform.position.x, entityTransform.position.y, entityTransform.position.z, entityTransform.orientation.y, entityTransform.orientation.x, entityTransform.orientation.z);
			EventData eventData = EventData(&moveEvent, sizeof(moveEvent));
			EventManager::GetInstance().TriggerEvent(EVENT_WORLD_PLAYER_MOVE, eventData);
			entityData.currentChunkPtr = chunkHandler->GetChunk(entityTransform.globalChunkPosition.x, entityTransform.globalChunkPosition.y, entityTransform.globalChunkPosition.z, false);
			if (!entityData.currentChunkPtr->IsReal()) {
				entityData.currentChunkPtr = nullptr;
			}
		}
	}

	Window& globalWindow = Window::GetInstance();
	glm::ivec2 windowSize = glm::ivec2(globalWindow.GetWidth(), globalWindow.GetHeight());
	UIScreen* inventoryUI = UI::GetInstance().GetScreen("ui/inventory");
	std::vector<TextureAtlasData> atlasData;
	atlasData.reserve(9 * 3);
	for (int slot = 0; slot < 27; slot++) {
		BlockType* blockType = BlockManager::GetInstance().GetBlockType(entityData.inventory.GetSlot(AssetStringID{"kiwicubed", "inventory_slot_" + fmt::format("{:02}", slot)})->itemStringID);
		atlasData.push_back(blockType->metaTextures[0].atlasData[0]);
	}
	Texture* atlas = assetManager.GetTextureAtlas(AssetStringID{"kiwicubed", "terrain_atlas"});
	Inventory* inventory = &entityData.inventory;
	inventoryUI->ClearCustomRenderCommands();
	inventoryUI->AddCustomRenderCommand([=]() {
		int containerX = (windowSize.x / 2) - 576;
		int containerY = (windowSize.y / 2) - 192;
		int slotIndex = 0;
		for (int imageY = 2; imageY <= 22; imageY += 10) {
			for (int imageX = 4; imageX <= 84; imageX += 10) {
				int slotX = containerX + (imageX * 12);
				int slotY = containerY + (imageY * 12);
				UIImage::Render(glm::vec2(slotX, slotY), glm::vec2(96, 96), atlasData[slotIndex], atlas);
				int itemCount = inventory->GetSlot({"kiwicubed", "inventory_slot_" + fmt::format("{:02}", slotIndex)})->itemCount;
				if (itemCount > 0) {
					UI::GetInstance().uiTextRenderer->RenderText(std::to_string(itemCount), static_cast<float>(slotX), static_cast<float>(slotY), 1, glm::vec3(1, 1, 1));
				}
				slotIndex++;
			}
		}
	});

	//std::cout << entityData.currentChunkPtr->GetHeightmapLevelAt(glm::vec2(entityData.localChunkPosition.x, entityData.localChunkPosition.z)) << std::endl;
}

void Player::QueryInputs() {
	glm::vec3 movementVector = glm::vec3(0);

	if (inputHandler.GetKeyState(GLFW_KEY_LEFT_CONTROL)) {
		speed = 10.0f;
	} else {
		speed = 5.0f;
	}

	bool shouldJump = false;

	if (playerData.gameMode == CREATIVE) {
		speed *= 3;

		glm::vec3 forward = entityTransform.orientation;
		forward.y = 0;
		forward = glm::normalize(forward);
		glm::vec3 right = glm::normalize(glm::cross(forward, entityTransform.upDirection));
		glm::vec3 up = glm::normalize(entityTransform.upDirection);

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

		if (glm::length(movementVector) > 0.0f) {
			movementVector = glm::normalize(movementVector) * speed;
		}
	} else {
		glm::vec3 forward = entityTransform.orientation;
		forward.y = 0;
		forward = glm::normalize(forward);
		glm::vec3 right = glm::normalize(glm::cross(forward, entityTransform.upDirection));

		if (inputHandler.GetKeyState(GLFW_KEY_W) && !inInterface) {
			movementVector += forward;
		}
		if (inputHandler.GetKeyState(GLFW_KEY_A) && !inInterface) {
			movementVector += -right;
		}
		if (inputHandler.GetKeyState(GLFW_KEY_S) && !inInterface) {
			movementVector += -forward;
		}
		if (inputHandler.GetKeyState(GLFW_KEY_D) && !inInterface) {
			movementVector += right;
		}
		if (inputHandler.GetKeyState(GLFW_KEY_SPACE) && entityData.isGrounded && !inInterface) {
			shouldJump = true;
			entityData.isJumping = true;
			jumpStart = std::chrono::steady_clock::now();
		}

		if (glm::length(movementVector) > 0.0f) {
			movementVector = glm::normalize(movementVector) * speed;
		}

		if (shouldJump) {
			movementVector.y = sqrt(8 * 9.81f * entityData.jumpHeight);
		} else {
			movementVector.y = entityTransform.velocity.y;
		}
	}

	entityTransform.velocity = movementVector;
}

void Player::MouseButtonCallback(int button) {
	// will be reworked much later probably
	if (!UI::GetInstance().IsDisabled()) {
		return;
	}

	glm::ivec3 chunkPosition = glm::ivec3(-1, -1, -1);
	glm::ivec3 blockPosition = glm::ivec3(-1, -1, -1);
	bool hit = 0;
	BlockRayHit rayHit = Physics::RaycastWorld(entityTransform.position, entityTransform.orientation, 500, *chunkHandler, blockPosition, chunkPosition, hit);
	if (rayHit.hit) {
		if (button == 0) {
			Block& block = chunkHandler->GetChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z, false)->GetBlock(blockPosition.x, blockPosition.y, blockPosition.z);
			BlockType* blockType = BlockManager::GetInstance().GetBlockType(block.blockID);
			EventWorldPlayerBlock blockEvent = EventWorldPlayerBlock(BLOCK_MINED, protectedEntityData.AUID, chunkPosition.x, chunkPosition.y, chunkPosition.z, blockPosition.x, blockPosition.y, blockPosition.z, blockType->blockStringID, AssetStringID{"kiwicubed", "block/air"});
			EventData eventData = EventData{&blockEvent, sizeof(blockEvent)};
			EventManager::GetInstance().TriggerEvent(EVENT_WORLD_PLAYER_BLOCK, eventData);
			entityData.inventory.AddItem(InventorySlot{*BlockManager::GetInstance().GetStringID(block.GetBlockID()), 1});
			chunkHandler->RemoveBlock(chunkPosition.x, chunkPosition.y, chunkPosition.z, blockPosition.x, blockPosition.y, blockPosition.z);
			if (blockPosition.x == 0 || blockPosition.x == chunkSize - 1 || blockPosition.y == 0 || blockPosition.y == chunkSize - 1 || blockPosition.z == 0 || blockPosition.z == chunkSize - 1) {
				chunkHandler->RemeshChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z, false);
				if (blockPosition.x == 0 || blockPosition.x == chunkSize - 1) {
					chunkHandler->RemeshChunk(chunkPosition.x - 1, chunkPosition.y, chunkPosition.z, false);
				}
				if (blockPosition.y == 0 || blockPosition.y == chunkSize - 1) {
					chunkHandler->RemeshChunk(chunkPosition.x, chunkPosition.y - 1, chunkPosition.z, false);
				}
				if (blockPosition.z == 0 || blockPosition.z == chunkSize - 1) {
					chunkHandler->RemeshChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z - 1, false);
				}
				if (blockPosition.x == chunkSize - 1) {
					chunkHandler->RemeshChunk(chunkPosition.x + 1, chunkPosition.y, chunkPosition.z, false);
				}
				if (blockPosition.y == chunkSize - 1) {
					chunkHandler->RemeshChunk(chunkPosition.x, chunkPosition.y + 1, chunkPosition.z, false);
				}
				if (blockPosition.z == chunkSize - 1) {
					chunkHandler->RemeshChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z + 1, false);
				}
			} else {
				chunkHandler->RemeshChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z, false);
			}
		} else if (button == 1) {
			glm::ivec3 chunkPosition = rayHit.fullBlockPosition.chunkPosition;
			glm::ivec3 blockPosition = rayHit.fullBlockPosition.blockPosition;
			glm::ivec3 placingBlockPosition = glm::ivec3(blockPosition);
			glm::ivec3 placingChunkPosition = glm::ivec3(chunkPosition);
			bool remesh = false;
			switch (rayHit.faceHitIndex) {
				case LEFT:
					if (blockPosition.x - 1 < 0) {
						placingBlockPosition.x = chunkSize - 1;
						placingChunkPosition.x -= 1;
						remesh = true;
					} else {
						placingBlockPosition.x -= 1;
						placingChunkPosition.x = chunkPosition.x;
					}
					break;
				case RIGHT:
					if (blockPosition.x + 1 == chunkSize - 1) {
						placingBlockPosition.x = 0;
						placingChunkPosition.x += 1;
						remesh = true;
					} else {
						placingBlockPosition.x += 1;
						placingChunkPosition.x = chunkPosition.x;
					}
					break;
				case BOTTOM:
					if (blockPosition.y - 1 < 0) {
						placingBlockPosition.y = chunkSize - 1;
						placingChunkPosition.y -= 1;
						remesh = true;
					} else {
						placingBlockPosition.y -= 1;
						placingChunkPosition.y = chunkPosition.y;
					}
					break;
				case TOP:
					if (blockPosition.y + 1 == chunkSize - 1) {
						placingBlockPosition.y = 0;
						placingChunkPosition.y += 1;
						remesh = true;
					} else {
						placingBlockPosition.y += 1;
						placingChunkPosition.y = chunkPosition.y;
					}
					break;
				case BACK:
					if (blockPosition.z - 1 < 0) {
						placingBlockPosition.z = chunkSize - 1;
						placingChunkPosition.z -= 1;
						remesh = true;
					} else {
						placingBlockPosition.z -= 1;
						placingChunkPosition.z = chunkPosition.z;
					}
					break;
				case FRONT:
					if (blockPosition.z + 1 == chunkSize - 1) {
						placingBlockPosition.z = 0;
						placingChunkPosition.z += 1;
						remesh = true;
					} else {
						placingBlockPosition.z += 1;
						placingChunkPosition.z = chunkPosition.z;
					}
					break;
			}
			bool emptyBlock = chunkHandler->GetChunk(placingChunkPosition.x, placingChunkPosition.y, placingChunkPosition.z, false)->GetBlock(placingBlockPosition.x, placingBlockPosition.y, placingBlockPosition.z).IsAir();
			bool collidesEntity = Physics::CollideBlock(*this, FullBlockPosition{placingBlockPosition, placingChunkPosition}, false);
			if (emptyBlock && !collidesEntity) {
				//EventManager::GetInstance().TriggerEvent("event/player_placed_block");
				int usingSlotIndex = -1;
				int newBlockID = 0;
				for (int slotIndex = 0; slotIndex < 27; slotIndex++) {
					AssetStringID slotStringID = AssetStringID{"kiwicubed", "inventory_slot_" + fmt::format("{:02}", slotIndex)};
					InventorySlot* slot = entityData.inventory.GetSlot(slotStringID);
					if (slot->itemStringID.assetName == "block/air") {
						continue;
					}
					InventorySlot newSlot = *slot;
					newBlockID = *BlockManager::GetInstance().GetNumericalID((*entityData.inventory.GetSlot(slotStringID)).itemStringID);
					newSlot.itemCount--;
					if (newSlot.itemCount == 0) {
						newSlot.itemStringID = AssetStringID{"kiwicubed", "block/air"};
					}
					entityData.inventory.SetSlot(slotStringID, newSlot);
					usingSlotIndex = slotIndex;
					break;
				}
				if (usingSlotIndex == -1) {
					return;
				}
				chunkHandler->AddBlock(placingChunkPosition.x, placingChunkPosition.y, placingChunkPosition.z, placingBlockPosition.x, placingBlockPosition.y, placingBlockPosition.z, newBlockID);
				chunkHandler->RemeshChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z, false);
				if (remesh) {
					chunkHandler->RemeshChunk(placingChunkPosition.x, placingChunkPosition.y, placingChunkPosition.z, false);
				}
			}
		}
	}
}

void Player::QueryMouseInputs() {
	Window& window = camera->GetWindow();
	if (!window.GetFocused() || !UI::GetInstance().IsDisabled()) {
		return;
	}

	if (inputHandler.GetKeyState(GLFW_KEY_MINUS)) {
		entityStats.health -= 0.1f;
	}

	// Does some absolute magic to rotate the camera correctly
	double mouseX;
	double mouseY;

	glfwGetCursorPos(window.GetGLFWWindow(), &mouseX, &mouseY);

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
		entityTransform.orientation = glm::normalize(facing);
	}

	// We don't want anyone to be able to move the mouse off the screen, that would be very very very bad and horrible and would make the game absolutely unplayable
	glfwSetCursorPos(window.GetGLFWWindow(), (static_cast<float>(window.GetWidth()) / 2.0f), (static_cast<float>(window.GetHeight()) / 2.0f));

	oldMouseX = mouseX;
	oldMouseY = mouseY;
}

void Player::SetPosition(float newPlayerX, float newPlayerY, float newPlayerZ) {
	OVERRIDE_LOG_NAME("Player Update");
	if (!camera) {
		WARN("Trying to update player without a camera, aborting");
		return;
	}
	entityTransform.position = glm::vec3(newPlayerX, newPlayerY, newPlayerZ);
	camera->UpdateMatrix(80.0f, 0.1f, 1000.0f, entityTransform.position, entityTransform.orientation, entityTransform.upDirection);
}

glm::vec3 Player::GetPosition() const {
	return glm::vec3(entityTransform.position.x, entityTransform.position.y, entityTransform.position.z);
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
	camera->UpdateMatrix(fov, 0.1f, 1000.0f, entityTransform.position, entityTransform.orientation, entityTransform.upDirection);
	camera->SetCameraMatrix(shader);
}

void Player::Delete() {
	for (unsigned int id : inputCallbackIDs) {
		inputHandler.DeregisterCallback(id, "Player");
	}
	inputCallbackIDs.clear();
}