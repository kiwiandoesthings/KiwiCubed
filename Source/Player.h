#pragma once

#include <glad/glad.h>

#include "Camera.h"
#include "Entity.h"


class Player : public Entity {
	public:
		float yaw = -90.0f;
		float pitch;
		float roll;

		int width;
		int height;

		float speed = 0.004f;
		float sensitivity = 100.0f;

		Player() : Entity(), yaw(0), pitch(0), roll(0), width(640), height(480), chunkHandler(chunkHandler) {}
		Player(int playerX, int playerY, int playerZ, ChunkHandler& chunkHandler);
	
		void Setup(Window& window);
	
		void Update();
		void QueryInputs();
		void MouseButtonCallback();
		void QueryMouseInputs();
	
		void SetPosition(Window* window, int playerX, int playerY, int playerZ);
		const std::tuple<int, int, int> GetPosition();
	
		void UpdateShader(Shader& shader, const char* uniform);
		void UpdateCameraMatrix(Shader& shader);
	
		void Delete();
	
	private:
		std::shared_ptr<Camera> camera;
		InputHandler inputHandler = InputHandler();
		ChunkHandler& chunkHandler;
		std::unordered_map<std::tuple<int, int, int>, Chunk, TripleHash> chunks;
};