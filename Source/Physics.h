#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <ImGui.h>

#include "ChunkHandler.h"


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

void ApplyPhysics(Entity& entity, PhysicsBoundingBox physicsBoundingBox, ChunkHandler& chunkHandler);