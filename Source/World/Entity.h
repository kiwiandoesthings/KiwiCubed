#pragma once

#include <glm/vec3.hpp>

#include <iostream>

#include "Physics.h"


struct EntityStats {
	float health;
	unsigned int armor;
};

struct EntityData {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 orientation = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::ivec3 globalChunkPosition = glm::ivec3(0, 0, 0);
	glm::ivec3 localChunkPosition = glm::ivec3(0, 0, 0);

	float terminalVelocity = 0.4f;

	PhysicsBoundingBox physicsBoundingBox = PhysicsBoundingBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	InteractionBoundingBox interactionBoundingBox = InteractionBoundingBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	const char* name = "";

	bool applyGravity = false;
	bool applyCollision = false;
};

struct ProtectedEntityData {
	// For use later (most likely for multiplayer)
	const char* UUID = "";
};

class Entity {
	protected:
		EntityStats entityStats;
		EntityData entityData;


	public:
		Entity() : entityStats(EntityStats()), entityData(EntityData()) {}
		Entity(float entityX, float entityY, float entityZ);
	
		virtual EntityStats GetEntityStats() const;
		virtual EntityData GetEntityData() const;
	
		virtual void SetEntityStats(EntityStats newEntityStats);
		virtual void SetEntityData(EntityData newEntityData);
	
		virtual void DamageEntity(float damage);

		virtual void Update();
	
		virtual void Render();
	
		virtual void Delete();
};