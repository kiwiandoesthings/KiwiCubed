#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <imgui.h>

#include "ChunkHandler.h"
#include "Globals.h"

class Entity;

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

inline int PositiveModulo(int a, int b) {
	return (a % b + b) % b;
}

inline int FloorMod(int a, int b) {
    int m = a % b;
    return (m < 0) ? m + b : m;
}

bool ApplyPhysics(Entity& entity, ChunkHandler& chunkHandler, bool applyGravity, bool applyCollision);
bool RaycastWorld(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, int maxDistance, ChunkHandler& chunkHandler, glm::ivec3& hitBlockPosition, glm::ivec3& hitChunkPosition, bool& isHit);
bool GetGrounded(Entity& entity, ChunkHandler& chunkHandler);