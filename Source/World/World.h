#pragma once

#include "GLError.h"
#include <glad/glad.h>

#include <atomic>
#include <chrono>
#include <map>
#include <thread>
#include <unordered_set>
#include <vector>

#include <glm/gtx/extend.hpp>

#include "ChunkHandler.h"
#include "Player.h"
#include "Shader.h"
#include "ThreadPool.h"


class Chunk;
class SingleplayerHandler;

struct ChunkData {
	glm::vec3 position;
};


class World {
	public:
		Player player = Player(0, 0, 0, chunkHandler);

		unsigned int totalChunks;
		float totalMemoryUsage;

		unsigned int generationQueuedChunks = 0;
		unsigned int meshingQueuedChunks = 0;

		World() : totalChunks(0), totalMemoryUsage(0), singleplayerHandler(singleplayerHandler), shouldTick(false), tickIntervalMs(50), worldSize(5), chunkHandler(*this) {}
		World(unsigned int worldSize, SingleplayerHandler* singleplayerHandler);

		void Setup(Window& window);
		void SetupRenderComponents();

		void Render(Shader shaderProgram);
		void GenerateWorld();
		void GenerateChunk(int chunkX, int chunkY, int chunkZ, Chunk& chunk, bool updateCallerChunk, Chunk& callerChunk);

		void GenerateChunksAroundPosition(Event& event, unsigned short horizontalRadius = 0, unsigned short verticalRadius = 0);

		void Update();

		void DisplayImGui(unsigned int option);

		Chunk GetChunk(int chunkX, int chunkY, int chunkZ);
		Entity GetEntity(std::string uuid);

		std::vector<float>& GetChunkDebugVisualizationVertices();
		std::vector<GLuint>& GetChunkDebugVisualizationIndices();

		std::vector<glm::vec4>& GetChunkOrigins();

		void Tick();
		bool StartTickThread();
		bool StopTickThread();

		void Delete();

	private:
		SingleplayerHandler* singleplayerHandler;
		Chunk defaultChunk = Chunk(0, 0, 0);
		
		std::atomic<bool> shouldTick;
		std::thread tickThread;
		std::mutex tickThreadMutex;
		int tickIntervalMs = 50;
		unsigned int totalTicks = 0;
		unsigned int ticksPerSecond = 0;
		std::chrono::steady_clock::time_point tpsStartTime = std::chrono::steady_clock::now();

		std::queue<glm::ivec3> chunkGenerationQueue;
		std::unordered_set<std::tuple<int, int, int>, TripleHash> chunkGenerationSet;
		std::queue<glm::ivec3> chunkMeshingQueue;
		std::unordered_set<std::tuple<int, int, int>, TripleHash> chunkMeshingSet;
		unsigned short playerChunkGenerationRadius = 2;

		ThreadPool chunkGenerationThreads = ThreadPool(4);

		bool isWorldAllocated = false;
		bool isWorldGenerated = false;
		unsigned int worldSize;
		ChunkHandler chunkHandler;

		Renderer renderer;

		std::vector<GLfloat> chunkDebugVisualizationVertices;
		std::vector<GLuint> chunkDebugVisualizationIndices;
		std::vector<glm::ivec3> chunkDebugVisualizationColors;

		std::vector<glm::vec4> chunkOrigins;

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