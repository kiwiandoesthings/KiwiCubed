#pragma once

#include "GLError.h"
#include <glad/glad.h>

#include <atomic>
#include <chrono>
#include <map>
#include <thread>
#include <vector>

#include "ChunkHandler.h"
#include "Player.h"
#include "Shader.h"


class Chunk;
class SingleplayerHandler;

struct ChunkData {
	glm::vec3 position;
};


class World {
	public:
		Player player = Player(36, 100, 80, chunkHandler);
		unsigned int totalChunks;

		World() : shouldTick(false), tickIntervalMs(50), chunkHandler(*this), totalChunks(0), totalMemoryUsage(0), worldSize(5), singleplayerHandler(singleplayerHandler) {}
		World(unsigned int worldSize, SingleplayerHandler& singleplayerHandler);

		void Setup(Window& window);
		void SetupRenderComponents();

		void Render(Shader shaderProgram);
		void GenerateWorld();
		void GenerateChunk(int chunkX, int chunkY, int chunkZ, Chunk& chunk, bool updateCallerChunk, Chunk& callerChunk);

		void Update(Window* window);

		void DisplayImGui(unsigned int option);

		Chunk GetChunk(int chunkX, int chunkY, int chunkZ);
		Entity GetEntity(std::string uuid);

		void Tick();
		bool StartTickThread();
		bool StopTickThread();

		void Delete();

	private:
		SingleplayerHandler& singleplayerHandler;

		std::atomic<bool> shouldTick;
		std::thread TickThread;
		std::mutex TickThreadMutex;
		int tickIntervalMs = 50;
		unsigned int totalTicks = 0;
		unsigned int ticksPerSecond = 0;
		std::chrono::steady_clock::time_point tpsStartTime = std::chrono::high_resolution_clock::now();

		bool isWorldAllocated = false;
		bool isWorldGenerated = false;
		unsigned int worldSize;
		ChunkHandler chunkHandler;
		float totalMemoryUsage;

		Renderer renderer;

		// Batch rendering later..?
		//std::vector<GLfloat> vertices;
		//std::vector<GLuint> indices;
		//unsigned int latestChunkVertexOffset = 0;
		//unsigned int latestChunkIndexOffset = 0;

		//IndexBufferObject indexBufferObject;
		//VertexArrayObject vertexArrayObject;
		//VertexBufferObject vertexBufferObject = VertexBufferObject(/*"world*/);

		void RunTickThread();
};