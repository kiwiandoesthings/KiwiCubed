#pragma once

#include "GLError.h"
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

		Player() : width(640), height(480), yaw(0), pitch(0), roll(0), Entity(), chunkHandler(chunkHandler) {}
		Player(int playerX, int playerY, int playerZ, ChunkHandler& chunkHandler);
	
		void Setup(Window& window);
	
		void Update(Window* window);
		void UpdateShader(Shader& shader, const char* uniform);
		void QueryInputs();
		void MouseButtonCallback();
		void QueryMouseInputs();
	
		void SetPosition(Window* window, int playerX, int playerY, int playerZ);
		const std::tuple<int, int, int> GetPosition();
	
		void UpdateCameraMatrix(Shader& shader, const char* uniform);
	
		void Delete();
	
	private:
		std::shared_ptr<Camera> camera;
		InputHandler inputHandler = InputHandler();
		ChunkHandler& chunkHandler;
		std::unordered_map<std::tuple<int, int, int>, Chunk, TripleHash> chunks;
};