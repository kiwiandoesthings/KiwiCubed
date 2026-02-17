#pragma once

#include <cmath>
#include <glm/vec3.hpp>

#include <array>
#include <iomanip>
#include <iostream>
#include <random>

#include "Inventory.h"
#include "Physics.h"
#include "Utils.h"


class World;


struct EntityStats {
	float health;
	unsigned int armor;
};

struct EntityData {
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

struct EntityTransform {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 orientation = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::ivec3 globalChunkPosition = glm::ivec3(0, 0, 0);
	glm::ivec3 localChunkPosition = glm::ivec3(0, 0, 0);

	Vec3 GetPosition() const {
		return Vec3(position.x, position.y, position.z);
	}
	void SetPosition(const Vec3& vec) {
		position = glm::vec3(vec.x, vec.y, vec.z);
	}
	Vec3 GetOrientation() const {
		return Vec3(orientation.x, orientation.y, orientation.z);
	}
	void SetOrientation(const Vec3& vec) {
		orientation = glm::vec3(vec.x, vec.y, vec.z);
	}
	Vec3 GetUpDirection() const {
		return Vec3(upDirection.x, upDirection.y, upDirection.z);
	}
	void SetUpDirection(const Vec3& vec) {
		upDirection = glm::vec3(vec.x, vec.y, vec.z);
	}
	Vec3 GetVelocity() const {
		return Vec3(velocity.x, velocity.y, velocity.z);
	}
	void SetVelocity(const Vec3& vec) {
		velocity = glm::vec3(vec.x, vec.y, vec.z);
	}

	Vec3 GetGlobalChunkPosition() const {
		return Vec3(globalChunkPosition.x, globalChunkPosition.y, globalChunkPosition.z);
	}
	Vec3 GetLocalChunkPosition() const {
		return Vec3(localChunkPosition.x, localChunkPosition.y, localChunkPosition.z);
	}
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
	unsigned long long AUID = 0;
};


class Entity {
	protected:
		EntityStats entityStats;
		EntityData entityData;
		EntityTransform entityTransform;
		EntityRenderData entityRenderData;
		ProtectedEntityData protectedEntityData;

	public:
		Entity(float entityX, float entityY, float entityZ, World* world);

		void SetupRenderComponents(AssetStringID modelID, AssetStringID atlasID, AssetStringID textureID);
	
		EntityStats GetEntityStats() const;
		EntityData GetEntityData() const;
		EntityTransform GetEntityTransform() const;
		ProtectedEntityData GetProtectedEntityData() const;
	
		void SetEntityStats(EntityStats newEntityStats);
		void SetEntityData(EntityData newEntityData);
		void SetEntityTransform(EntityTransform newEntityTransform);
	
		virtual void DamageEntity(float damage);

		virtual void Update();
	
		virtual void Render();

		unsigned long long CreateAUID();
	
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