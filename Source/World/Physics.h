#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <imgui.h>

#include "ChunkHandler.h"
#include "Globals.h"


class Entity;
struct EntityData;


struct BoundingBox {
	glm::vec3 corner1;
	glm::vec3 corner2;

	float width;
	float height;
	float length;

	glm::vec3 Midpoint(const glm::vec3 position) const {
		return glm::vec3((corner1.x + corner2.x) / 2,(corner1.y + corner2.y) / 2, (corner1.z + corner2.z) / 2);
	}

	BoundingBox(glm::vec3 corner1, glm::vec3 corner2) : corner1(corner1), corner2(corner2), width(abs(corner1.x - corner2.x)), height(abs(corner1.y - corner2.y)), length(abs(corner1.z - corner2.z)) {}
};

struct FullBlockPosition {
	glm::ivec3 blockPosition;
	glm::ivec3 chunkPosition;

	FullBlockPosition() : blockPosition(glm::ivec3(-1)), chunkPosition(glm::ivec3(-1)) {}
	FullBlockPosition(glm::ivec3 blockPosition, glm::ivec3 chunkPosition) : blockPosition(blockPosition), chunkPosition(chunkPosition) {}
};

struct BlockRayHit {
	bool hit;
	FullBlockPosition fullBlockPosition;
	unsigned char faceHitIndex;

	// Should only be used when hit is false
	BlockRayHit(bool hit) : hit(false), fullBlockPosition(FullBlockPosition{}), faceHitIndex(0) {}
};


class Physics {
	public:
		static Physics& GetInstance();

		Physics();

		static void Initialize();

		static bool ApplyPhysics(Entity& entity, ChunkHandler& chunkHandler, bool applyGravity, bool applyCollision);
		static BlockRayHit RaycastWorld(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, int maxDistance, ChunkHandler& chunkHandler, glm::ivec3& hitBlockPosition, glm::ivec3& hitChunkPosition, bool& isHit);
		static bool GetGrounded(Entity& entity, ChunkHandler& chunkHandler);
		static bool CollideBlock(Entity& entity, FullBlockPosition fullBlockPosition, bool resolveCollision);

		static inline int PositiveModulo(float value, int modulator) {
    		int newValue = static_cast<int>(std::floor(value));
    		int result = newValue % modulator;
    		return (result < 0) ? result + modulator : result;
		}

		static inline int FloorDiv(float value, int divisor) {
    		int newValue = static_cast<int>(std::floor(value));
    		int result = newValue / divisor;
    		if (value < 0 && newValue % divisor != 0) {
        		result -= 1;
    		}
    		return result;
		}

	private:
    	~Physics() = default;

    	Physics(const Physics&) = delete;
    	Physics& operator=(const Physics&) = delete;

		static bool CollideAxis(unsigned char axis, EntityTransform& newTransform, EntityData& entityData, ChunkHandler& chunkHandler);
		static float CollideAxisFloat(unsigned char axis, EntityTransform& newEntityData, EntityData& entityData, ChunkHandler& chunkHandler);
		static bool ApplyTerrainCollision(EntityTransform& newEntityData, ChunkHandler& chunkHandler);

		static std::vector<FullBlockPosition> blockCollisionQueue;
};