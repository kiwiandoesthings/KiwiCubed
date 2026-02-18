#pragma once

#include <chrono>
#include <glad/glad.h>
#include <klogger.hpp>

#include "fmt/core.h"
#include "fmt/format.h"

#include "Block.h"
#include "Camera.h"
#include "Entity.h"
#include "Events.h"
#include "TextRenderer.h"
#include "UI.h"


enum GameMode {
	SURVIVAL,
	CREATIVE
};


struct PlayerData {
	GameMode gameMode = CREATIVE;
};


class Player : public Entity {
	public:
		float yaw = -90.0f;
		float pitch;
		float roll;

		float fov = 80.0f;

		int width;
		int height;

		float speed = 0.004f;
		float sensitivity = 100.0f;

		Player(int playerX, int playerY, int playerZ, World* world, unsigned long long playerAUID, EntityType* type);
	
		void Setup();
	
		void Update();
		void QueryInputs();
		void MouseButtonCallback(int button);
		void QueryMouseInputs();
	
		void SetPosition(float playerX, float playerY, float playerZ);
		glm::vec3 GetPosition() const;

		GameMode GetGameMode() const;
		bool SetGameMode(GameMode gameMode);
		std::string GetGameModeString() const;
	
		void UpdateCameraMatrix(Shader& shader);
	
		void Delete();
	
	private:
		PlayerData playerData = PlayerData();
		std::shared_ptr<Camera> camera;
		glm::vec3 cameraOffset = glm::vec3(0.0f, 1.62f, 0.0f);
		InputHandler inputHandler = InputHandler("Player");
		std::vector<unsigned int> inputCallbackIDs;
		World* world;
		ChunkHandler* chunkHandler;

		float oldMouseX = 0;
		float oldMouseY = 0;

		bool inInterface = false;

		std::chrono::time_point<std::chrono::steady_clock> jumpStart;
};