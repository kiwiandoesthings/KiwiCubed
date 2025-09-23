#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <imgui.h>

#include "ChunkHandler.h"
#include "Globals.h"

class Entity;
struct EntityData;

struct PhysicsBoundingBox {
	glm::vec3 corner1;
	glm::vec3 corner2;

	PhysicsBoundingBox(glm::vec3 corner1, glm::vec3 corner2) : corner1(corner1), corner2(corner2) {}
};

// For the future
struct InteractionBoundingBox {
	glm::vec3 corner1;
	glm::vec3 corner2;

	InteractionBoundingBox(glm::vec3 corner1, glm::vec3 corner2) : corner1(corner1), corner2(corner2) {}
};

struct FullBlockPosition {
	glm::ivec3 blockPosition;
	glm::ivec3 chunkPosition;

	FullBlockPosition(glm::ivec3 blockPosition, glm::ivec3 chunkPosition) : blockPosition(blockPosition), chunkPosition(chunkPosition) {}
};

class Physics {
	public:
		static Physics& GetInstance();

		Physics();

		static void Initialize();

		static bool ApplyPhysics(Entity& entity, ChunkHandler& chunkHandler, bool applyGravity, bool applyCollision);
		static bool RaycastWorld(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, int maxDistance, ChunkHandler& chunkHandler, glm::ivec3& hitBlockPosition, glm::ivec3& hitChunkPosition, bool& isHit);
		static bool GetGrounded(Entity& entity, ChunkHandler& chunkHandler);

		static inline int PositiveModulo(int a, int b) {
			return (a % b + b) % b;
		}

		static inline int FloorMod(int a, int b) {
    		int m = a % b;
    		return (m < 0) ? m + b : m;
		}

		static inline int FloorDiv(int a, int b) {
    		return (a >= 0) ? a / b : (a - b + 1) / b;
		}

	private:
    	~Physics() = default;

    	Physics(const Physics&) = delete;
    	Physics& operator=(const Physics&) = delete;

		static bool CollideAxis(unsigned char axis, EntityData& newEntityData, ChunkHandler& chunkHandler);
		static float CollideAxisFloat(unsigned char axis, EntityData& newEntityData, ChunkHandler& chunkHandler);
		static bool ApplyTerrainCollision(EntityData& newEntityData, ChunkHandler& chunkHandler);

		static std::vector<FullBlockPosition> blockCollisionQueue;
};