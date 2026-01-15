#pragma once

#include <cmath>
#include <glm/vec3.hpp>

#include <array>
#include <iomanip>
#include <iostream>
#include <random>

#include "Inventory.h"
#include "Physics.h"


class World;


struct EntityType {
	AssetStringID entityStringID;
	std::unordered_map<std::string, std::string> eventsToCallbacks;

	MetaEntityModel metaModel;
	MetaTexture metaTexture;
};

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

	// Used so physics and other systems don't have to constantly re-find the entity's current chunk
	Chunk* currentChunkPtr = nullptr;

	float terminalVelocity = 100.0f;

	BoundingBox physicsBoundingBox = BoundingBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	BoundingBox interactionBoundingBox = BoundingBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	std::string name = "";

	Inventory inventory;

	float moveSpeed = 1.0f;
	float jumpHeight = 1.25;

	float walkModifier = 1.0f;
	float jumpModifier = 1.0f;

	bool applyGravity = true;
	bool applyCollision = true;
	
	bool isGrounded = false;
	bool isJumping = false;

	bool isPlayer = false;
};

struct EntityRenderData {
	glm::vec3 oldPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 oldOrientation = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 oldUpDirection = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 oldVelocity = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 positionOffset = glm::vec3(0);
	glm::vec3 orientationOffset = glm::vec3(0);
	glm::vec3 oldPositionOffset = glm::vec3(0);
	glm::vec3 oldOrientationOffset = glm::vec3(0);
};

struct ProtectedEntityData {
	// For use later (most likely for multiplayer)
	std::string UUID = "";
};


class Entity {
	protected:
		EntityStats entityStats;
		EntityData entityData;
		EntityRenderData entityRenderData;
		ProtectedEntityData protectedEntityData;

	public:
		Entity(float entityX, float entityY, float entityZ, World* world);

		void SetupRenderComponents(AssetStringID modelID, AssetStringID atlasID, AssetStringID textureID);
	
		virtual EntityStats GetEntityStats() const;
		virtual EntityData GetEntityData() const;
		virtual ProtectedEntityData GetProtectedEntityData() const;
	
		virtual void SetEntityStats(EntityStats newEntityStats);
		virtual void SetEntityData(EntityData newEntityData);
	
		virtual void DamageEntity(float damage);

		virtual void Update();
	
		virtual void Render();

		std::string CreateUUID();
	
		virtual void Delete();
		
	private:
		World* world;

		Model entityModel;
		Texture* entityTexture;
		std::vector<TextureAtlasData>* entityTextureAtlasData;

		VertexBufferObject vertexBufferObject;
        VertexArrayObject vertexArrayObject;
        IndexBufferObject indexBufferObject;
};