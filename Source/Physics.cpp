#include "Physics.h"
#include "Entity.h"


void* operator new(size_t size) {
	//std::cout << "allocated " << size << " bytes" << std::endl;
	return malloc(size);
}


static void ApplyGravity(EntityData& newEntityData) {
	if (abs(newEntityData.velocity.y) < newEntityData.terminalVelocity) {
		newEntityData.velocity.y -= 0.00f;
	}
	else {
		if (newEntityData.velocity.y > 0) {
			newEntityData.velocity.y = newEntityData.terminalVelocity;
		}
		else if (newEntityData.velocity.y < 0) {
			newEntityData.velocity.y = -newEntityData.terminalVelocity;
		}
	}
}

static bool CollideBoundingBoxes(EntityData& newEntityData, const PhysicsBoundingBox& playerBoundingBox, const PhysicsBoundingBox& blockBoundingBox) {
	return false;
}

static int positiveModulo(int a, int b) {
	return (a % b + b) % b;
}


static bool ApplyTerrainCollision(EntityData& newEntityData, const PhysicsBoundingBox& physicsBoundingBox, ChunkHandler& chunkHandler) {
	glm::vec3 position = newEntityData.position;
	glm::vec3 chunkGlobalPosition = glm::vec3(static_cast<int>(position.x / 32), static_cast<int>(position.y / 32), static_cast<int>(position.z / 32));
	glm::vec3 chunkLocalPosition = glm::vec3(positiveModulo(static_cast<int>(position.x), 32), positiveModulo(static_cast<int>(position.y), 32), positiveModulo(static_cast<int>(position.z), 32));
	newEntityData.globalChunkPosition = chunkGlobalPosition;
	newEntityData.localChunkPosition = chunkLocalPosition;
	
	Chunk& chunk = chunkHandler.GetChunk(static_cast<int>(position.x / 32), static_cast<int>(position.y / 32), static_cast<int>(position.z / 32));
	if (!chunk.isGenerated) {
		return false;
	}
	Block& block = chunk.blocks[positiveModulo(static_cast<int>(position.x), 32)][positiveModulo(static_cast<int>(position.y), 32)][positiveModulo(static_cast<int>(position.z), 32)];
	glm::vec3 blockCenter = glm::vec3((static_cast<int>(position.x) % 32) - 0.5, (static_cast<int>(position.y) % 32) - 0.5, (static_cast<int>(position.z) % 32) - 0.5);
	//std::cout << blockCenter.x << " " << blockCenter.y << " " << blockCenter.z << std::endl;
	glm::vec3 min1 = glm::min(physicsBoundingBox.corner1 + newEntityData.position, physicsBoundingBox.corner2 + newEntityData.position);
	glm::vec3 max1 = glm::max(physicsBoundingBox.corner1 + newEntityData.position, physicsBoundingBox.corner2 + newEntityData.position);

	//std::cout << min1.x << " " << min1.y << " " << max1.x << " " << max1.y << std::endl;

	for (int x = 0; x < 32; ++x) {
		for (int y = 0; y < 32; ++y) {
			for (int z = 0; z < 32; ++z) {
				// Only check non-air blocks
				if (chunk.blocks[x][y][z].GetType() > 0) {
					glm::vec3 min2 = glm::vec3(x + (chunk.chunkX * chunkSize), y + (chunk.chunkY * chunkSize), z + (chunk.chunkZ * chunkSize));
					glm::vec3 max2 = glm::vec3(x + 1 + (chunk.chunkX * chunkSize), y + 1 + (chunk.chunkY * chunkSize), z + 1 + (chunk.chunkZ * chunkSize));

					bool isColliding =
						(min1.x <= max2.x && max1.x >= min2.x) &&
						(min1.y <= max2.y && max1.y >= min2.y) &&
						(min1.z <= max2.z && max1.z >= min2.z);

					if (isColliding) {
						std::cout << "Collision detected with block at: (" << x << ", " << y << ", " << z << ")" << std::endl;
						return true;
					}
				}
			}
		}
	}

	return false;
}

void ApplyPhysics(Entity& entity, PhysicsBoundingBox physicsBoundingBox, ChunkHandler& chunkHandler) {
	EntityData newEntityData = entity.GetEntityData();
	
	ApplyGravity(newEntityData);
	ApplyTerrainCollision(newEntityData, physicsBoundingBox, chunkHandler);
	
	//newEntityData.position += newEntityData.velocity;
	
	entity.SetEntityData(newEntityData);
}