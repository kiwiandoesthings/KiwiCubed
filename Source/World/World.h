#pragma once

#include "GLError.h"
#include <glad/glad.h>

#include <atomic>
#include <chrono>
#include <map>
#include <random>
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
		float totalMemoryUsage;

		World(unsigned int worldSizeHorizontal, unsigned int worldSizeVertical, SingleplayerHandler* singleplayerHandler);

		void Setup();
		void SetupRenderComponents();

		void Render(Shader shaderProgram);
		void GenerateWorld();
		void GenerateChunk(int chunkX, int chunkY, int chunkZ, Chunk* chunk, bool updateCallerChunk, Chunk* callerChunk);

		void RecalculateChunksToLoad(Event event, unsigned short horizontalRadius = 0, unsigned short verticalRadius = 0);
        void QueueMesh(glm::ivec3 chunkPosition, bool remesh);
		void QueueTickTask(std::function<void()> task);

		void SpawnItemFromBlock(glm::ivec3 chunkPosition, glm::ivec3 blockPosition, BlockType* blockType);

		void Update();

		void DisplayImGui(unsigned int option);

		ChunkHandler& GetChunkHandler();
		Chunk* GetChunk(int chunkX, int chunkY, int chunkZ);
		Entity* GetEntity(std::string uuid);

		//temporary - merge into GetEntity
		Player& GetPlayer();

		std::vector<float>& GetChunkDebugVisualizationVertices();
		std::vector<GLuint>& GetChunkDebugVisualizationIndices();

		std::vector<glm::vec4>& GetChunkOrigins();

		FastNoiseLite& GetNoise();

		void Tick();
		bool StartTickThread();
		bool StopTickThread();

		void Delete();

	private:		
		std::atomic<bool> shouldTick;
		std::thread tickThread;
		std::mutex tickThreadMutex;
		int tickIntervalMs = 50;
		unsigned int totalTicks = 0;
		float ticksPerSecond = 0.0f;
		float tickAccumulator = 0.0f;
		std::chrono::steady_clock::time_point tpsStartTime = std::chrono::steady_clock::now();

		std::mutex ChunkQueueMutex;
		std::vector<glm::ivec3> chunkGenerationQueue;
		std::unordered_set<std::tuple<int, int, int>, TripleHash> chunkGenerationSet;
		std::vector<glm::ivec3> chunkMeshingQueue;
		std::unordered_set<std::tuple<int, int, int>, TripleHash> chunkMeshingSet;
		std::queue<glm::ivec3> chunkUnloadingQueue;
		std::queue<std::function<void()>> tickTaskQueue;
		unsigned short playerChunkGenerationRadiusHorizontal = 8;
		unsigned short playerChunkGenerationRadiusVertical = 3;

		ThreadPool chunkGenerationThreads = ThreadPool(4);
		FastNoiseLite noise;

		bool isWorldAllocated = false;
		bool isWorldGenerated = false;
		unsigned int worldSizeHorizontal;
		unsigned int worldSizeVertical;
		Player player = Player(0, 0, 0, this);
		std::vector<std::unique_ptr<Entity>> entities;
		ChunkHandler chunkHandler;
		SingleplayerHandler* singleplayerHandler;

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