#pragma once

#include <chrono>
#include <glad/glad.h>
#include <klogger.hpp>

#include "Camera.h"
#include "Entity.h"
#include "Events.h"


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

		int width;
		int height;

		float speed = 0.004f;
		float sensitivity = 100.0f;

		Player() : world(world), Entity(0, 0, 0, world), yaw(0), pitch(0), roll(0), width(640), height(480) {}
		Player(int playerX, int playerY, int playerZ, World& world);
	
		void Setup();
	
		void Update();
		void QueryInputs();
		void MouseButtonCallback(int button);
		void QueryMouseInputs();
	
		void SetPosition(float playerX, float playerY, float playerZ);
		glm::vec3 GetPosition() const;

		GameMode GetGameMode() const;
		std::string GetGameModeString() const;
	
		void UpdateCameraMatrix(Shader& shader);
	
		void Delete();
	
	private:
		PlayerData playerData = PlayerData();
		std::shared_ptr<Camera> camera;
		InputHandler inputHandler = InputHandler("Player");
		std::vector<unsigned int> inputCallbackIDs;
		World& world;
		ChunkHandler* chunkHandler;
		std::unordered_map<std::tuple<int, int, int>, Chunk, TripleHash> chunks;

		float oldMouseX = 0;
		float oldMouseY = 0;

		bool inInterface = false;

		std::chrono::time_point<std::chrono::high_resolution_clock> jumpStart;
};