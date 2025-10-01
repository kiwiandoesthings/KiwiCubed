#include "Physics.h"
#include "Block.h"
#include "ChunkHandler.h"
#include "Entity.h"


Physics& Physics::GetInstance() {
    static Physics instance;
    return instance;
}

std::vector<FullBlockPosition> Physics::blockCollisionQueue;

Physics::Physics() {}

void Physics::Initialize() {
	blockCollisionQueue.reserve(3 * 3 * 3);
}

static void ApplyGravity(EntityData& newEntityData) {
	Globals& globals = Globals::GetInstance();
	float gravityPerSecond = 9.81f;
	// makeshift deltatime until render thread is set up. first frame after creating world,
	// deltatime is super high and you get like -60 velocity in 1 frame
    newEntityData.velocity.y -= gravityPerSecond * (60.0 / 1000.0);
}

static void ClipVelocity(EntityData& newEntityData, int axis) {
	for (int axis = 0; axis < 3; axis++)
	if (abs(newEntityData.velocity[axis]) > newEntityData.terminalVelocity) {
		if (newEntityData.velocity[axis] > 0) {
			newEntityData.velocity[axis] = newEntityData.terminalVelocity;
		} else {
			newEntityData.velocity[axis] = -newEntityData.terminalVelocity;
		}
	}
}

bool Physics::CollideAxis(unsigned char axis, EntityData& newEntityData, ChunkHandler& chunkHandler) {
	if (!newEntityData.currentChunkPtr->isGenerated) {
		return false;
	}

 	glm::vec3 min1 = glm::vec3(newEntityData.physicsBoundingBox.corner1.x + newEntityData.position.x, newEntityData.physicsBoundingBox.corner1.y + newEntityData.position.y, newEntityData.physicsBoundingBox.corner1.z + newEntityData.position.z);
	glm::vec3 max1 = glm::vec3(newEntityData.physicsBoundingBox.corner2.x + newEntityData.position.x, newEntityData.physicsBoundingBox.corner2.y + newEntityData.position.y, newEntityData.physicsBoundingBox.corner2.z + newEntityData.position.z);
	for (auto& blockPosition : blockCollisionQueue) {
		Chunk* targetChunk = nullptr;
		if (blockPosition.chunkPosition.x != newEntityData.globalChunkPosition.x || blockPosition.chunkPosition.y != newEntityData.globalChunkPosition.y || blockPosition.chunkPosition.z != newEntityData.globalChunkPosition.z) {
			targetChunk = &chunkHandler.GetChunk(blockPosition.chunkPosition.x, blockPosition.chunkPosition.y, blockPosition.chunkPosition.z, false);
			if (!targetChunk->isGenerated) {
				continue;
			}
		} else {
			targetChunk = newEntityData.currentChunkPtr;
		}

		Block& block = targetChunk->GetBlock(blockPosition.blockPosition.x, blockPosition.blockPosition.y, blockPosition.blockPosition.z);

		if (!block.IsAir()) {
			glm::vec3 min2 = glm::vec3(blockPosition.blockPosition.x + (blockPosition.chunkPosition.x * chunkSize), blockPosition.blockPosition.y + (blockPosition.chunkPosition.y * chunkSize), blockPosition.blockPosition.z + (blockPosition.chunkPosition.z * chunkSize));
			glm::vec3 max2 = min2 + glm::vec3(1.0f);
			
			bool isColliding =
				(min1.x < max2.x && max1.x > min2.x) &&
				(min1.y < max2.y && max1.y > min2.y) &&
				(min1.z < max2.z && max1.z > min2.z);

			if (isColliding) {
				float collision = min1[axis] - min2[axis];
				if (collision < 0) {
					newEntityData.position[axis] = min2[axis] - newEntityData.physicsBoundingBox.corner2[axis];
				} else {
					newEntityData.position[axis] = max2[axis] - newEntityData.physicsBoundingBox.corner1[axis];
				}
				newEntityData.velocity[axis] = 0;
				return true;
			}
		}
	}
	return false;
}

float Physics::CollideAxisFloat(unsigned char axis, EntityData& newEntityData, ChunkHandler& chunkHandler) {
	if (!newEntityData.currentChunkPtr->isGenerated) {
		std::cout << "no gen " << std::endl;
		return 0.0f;
	}

	glm::vec3 min1 = glm::vec3(newEntityData.physicsBoundingBox.corner1.x + newEntityData.position.x, newEntityData.physicsBoundingBox.corner1.y + newEntityData.position.y, newEntityData.physicsBoundingBox.corner1.z + newEntityData.position.z);
	glm::vec3 max1 = glm::vec3(newEntityData.physicsBoundingBox.corner2.x + newEntityData.position.x, newEntityData.physicsBoundingBox.corner2.y + newEntityData.position.y, newEntityData.physicsBoundingBox.corner2.z + newEntityData.position.z);
	for (auto& blockPosition : blockCollisionQueue) {
		Chunk* targetChunk = nullptr;
		if (blockPosition.chunkPosition.x != newEntityData.globalChunkPosition.x || blockPosition.chunkPosition.y != newEntityData.globalChunkPosition.y || blockPosition.chunkPosition.z != newEntityData.globalChunkPosition.z) {
			targetChunk = &chunkHandler.GetChunk(blockPosition.chunkPosition.x, blockPosition.chunkPosition.y, blockPosition.chunkPosition.z, false);
			if (!targetChunk->isGenerated) {
				std::cout << "no chunk " << std::endl;
				continue;
			}
		} else {
			targetChunk = newEntityData.currentChunkPtr;
		}

		Block& block = targetChunk->GetBlock(blockPosition.blockPosition.x, blockPosition.blockPosition.y, blockPosition.blockPosition.z);

		if (!block.IsAir()) {
			glm::vec3 min2 = glm::vec3(blockPosition.blockPosition.x + (blockPosition.chunkPosition.x * chunkSize), blockPosition.blockPosition.y + (blockPosition.chunkPosition.y * chunkSize), blockPosition.blockPosition.z + (blockPosition.chunkPosition.z * chunkSize));
			glm::vec3 max2 = min2 + glm::vec3(1.0f);

			bool isColliding =
				(min1.x < max2.x && max1.x > min2.x) &&
				(min1.y < max2.y && max1.y > min2.y) &&
				(min1.z < max2.z && max1.z > min2.z);

			if (isColliding) {
				float collision = min1[axis] - min2[axis];
				if (collision < 0) {
					glm::vec3 pos = newEntityData.position;
					newEntityData.position[axis] = min2[axis] - newEntityData.physicsBoundingBox.corner2[axis];
					std::cout << "moved from " << pos[axis] << " to " << newEntityData.position[axis] << " on axis " << std::to_string(axis) << std::endl;
				} else {
					glm::vec3 pos = newEntityData.position;
					newEntityData.position[axis] = max2[axis] - newEntityData.physicsBoundingBox.corner1[axis];
					std::cout << "moved from " << pos[axis] << " to " << newEntityData.position[axis] << " on axis " << std::to_string(axis) << std::endl;
				}
				newEntityData.velocity[axis] = 0;
				std::cout << "got y collision with " << collision << " amt at y " << newEntityData.localChunkPosition[axis] << " specifically " << newEntityData.position[axis] << " from block " << blockPosition.blockPosition.x << " " << blockPosition.blockPosition.y << " " << blockPosition.blockPosition.z << ", chunk " << blockPosition.chunkPosition.x << " " << blockPosition.chunkPosition.y << " " << blockPosition.chunkPosition.z << std::endl;
				return collision;
			}
		}
	}
	std::cout << "nothing at " << newEntityData.localChunkPosition.y << " specifically " << newEntityData.position.y << std::endl;
	return 0.0f;
}

bool Physics::ApplyTerrainCollision(EntityData& newEntityData, ChunkHandler& chunkHandler) {
	Globals& globals = Globals::GetInstance();

	blockCollisionQueue.clear();

	glm::vec3 minCorner = glm::min(newEntityData.physicsBoundingBox.corner1 + newEntityData.position, newEntityData.physicsBoundingBox.corner2 + newEntityData.position);
	glm::vec3 maxCorner = glm::max(newEntityData.physicsBoundingBox.corner1 + newEntityData.position, newEntityData.physicsBoundingBox.corner2 + newEntityData.position);

	for (int blockX = static_cast<int>(std::floor(minCorner.x)) - 1; blockX <= static_cast<int>(std::ceil(maxCorner.x)) + 1; ++blockX) {
		for (int blockY = static_cast<int>(std::floor(minCorner.y)) - 1; blockY <= static_cast<int>(std::ceil(maxCorner.y)) + 1; ++blockY) {
			for (int blockZ = static_cast<int>(std::floor(minCorner.z)) - 1; blockZ <= static_cast<int>(std::ceil(maxCorner.z)) + 1; ++blockZ) {
				glm::ivec3 chunkPosition = glm::ivec3(
					FloorDiv(blockX, 32),
					FloorDiv(blockY, 32),
					FloorDiv(blockZ, 32)
				);

				glm::ivec3 blockPosition = glm::ivec3(
					PositiveModulo(blockX, 32),
					PositiveModulo(blockY, 32),
					PositiveModulo(blockZ, 32)
				);

				blockCollisionQueue.emplace_back(FullBlockPosition(blockPosition, chunkPosition));
			}
		}
	}

	for (FullBlockPosition blockPos : blockCollisionQueue) {
		//std::cout << blockPos.chunkPosition.x << " " << blockPos.chunkPosition.y << " " << blockPos.chunkPosition.z << ", " << blockPos.blockPosition.x << " " << blockPos.blockPosition.y << " "<< blockPos.blockPosition.z << std::endl;
	}
	//std::cout << "done" << std::endl;

	newEntityData.position.x += newEntityData.velocity.x * globals.deltaTime;
	bool xAxis = CollideAxis(0, newEntityData, chunkHandler);
	newEntityData.position.y += newEntityData.velocity.y * globals.deltaTime;
	float yAxis = CollideAxisFloat(1, newEntityData, chunkHandler);
	newEntityData.position.z += newEntityData.velocity.z * globals.deltaTime;
	bool zAxis = CollideAxis(2, newEntityData, chunkHandler);

	newEntityData.isGrounded = (yAxis > 0);

	if (xAxis || yAxis != 0 || zAxis) {
		return true;
	}

	return false;
}

bool Physics::ApplyPhysics(Entity& entity, ChunkHandler& chunkHandler, bool applyGravity, bool applyCollision) {
	EntityData newEntityData = entity.GetEntityData();

	bool grounded = false;

	ClipVelocity(newEntityData, 0); 
	ClipVelocity(newEntityData, 1); 
	ClipVelocity(newEntityData, 2);

	if (applyGravity) {
		ApplyGravity(newEntityData);
		ClipVelocity(newEntityData, 1);
	}
	if (applyCollision) {
		ApplyTerrainCollision(newEntityData, chunkHandler);
	} else {
		newEntityData.position.x += newEntityData.velocity.x;
		newEntityData.position.y += newEntityData.velocity.y;
		newEntityData.position.z += newEntityData.velocity.z;
	}

	entity.SetEntityData(newEntityData);
	return grounded;
}

// Literally what the fuck even is this functionn I hate raycasting so much
bool Physics::RaycastWorld(const glm::vec3& origin, const glm::vec3& direction, int maxDistance, ChunkHandler& chunkHandler, glm::ivec3& blockHitPosition, glm::ivec3& chunkHitPosition, bool& isHit) {
	glm::ivec3 currentBlock = glm::floor(origin);
	glm::ivec3 currentChunk = glm::ivec3(glm::floor(float(currentBlock.x) / float(chunkSize)), glm::floor(float(currentBlock.y) / float(chunkSize)),glm::floor(float(currentBlock.z) / float(chunkSize)));

	glm::ivec3 stepDirection = glm::ivec3(direction.x > 0 ? 1 : -1, direction.y > 0 ? 1 : -1, direction.z > 0 ? 1 : -1);

	glm::vec3 stepSize = glm::vec3(std::abs(1.0f / direction.x), std::abs(1.0f / direction.y), std::abs(1.0f / direction.z));
	glm::vec3 distanceToBlockBoundary = glm::vec3((currentBlock.x + (stepDirection.x > 0 ? 1 : 0) - origin.x) / direction.x, (currentBlock.y + (stepDirection.y > 0 ? 1 : 0) - origin.y) / direction.y, (currentBlock.z + (stepDirection.z > 0 ? 1 : 0) - origin.z) / direction.z);

	for (int i = 0; i < maxDistance; ++i) {
		glm::ivec3 localBlockPos = glm::ivec3(PositiveModulo(currentBlock.x, chunkSize), PositiveModulo(currentBlock.y, chunkSize), PositiveModulo(currentBlock.z, chunkSize));

		Chunk chunk = chunkHandler.GetChunk(currentChunk.x, currentChunk.y, currentChunk.z, false);
		if (chunk.isGenerated) {
			Block& block = chunk.GetBlock(localBlockPos.x, localBlockPos.y, localBlockPos.z);
			if (!block.IsAir()) {
				blockHitPosition = glm::ivec3(PositiveModulo(currentBlock.x, chunkSize), PositiveModulo(currentBlock.y, chunkSize), PositiveModulo(currentBlock.z, chunkSize));
				chunkHitPosition = currentChunk;
				isHit = true;
				return true;
			}

			if (distanceToBlockBoundary.x < distanceToBlockBoundary.y && distanceToBlockBoundary.x < distanceToBlockBoundary.z) {
				currentBlock.x += stepDirection.x;
				distanceToBlockBoundary.x += stepSize.x;

				int newChunkX = static_cast<int>(glm::floor(float(currentBlock.x) / float(chunkSize)));
				if (newChunkX != currentChunk.x) {
					currentChunk.x = newChunkX;
				}
			}
			else if (distanceToBlockBoundary.y < distanceToBlockBoundary.z) {
				currentBlock.y += stepDirection.y;
				distanceToBlockBoundary.y += stepSize.y;

				int newChunkY = static_cast<int>(glm::floor(float(currentBlock.y) / float(chunkSize)));
				if (newChunkY != currentChunk.y) {
					currentChunk.y = newChunkY;
				}
			}
			else {
				currentBlock.z += stepDirection.z;
				distanceToBlockBoundary.z += stepSize.z;

				int newChunkZ = static_cast<int>(glm::floor(float(currentBlock.z) / float(chunkSize)));
				if (newChunkZ != currentChunk.z) {
					currentChunk.z = newChunkZ;
				}
			}
		}
	}

	blockHitPosition = glm::ivec3(PositiveModulo(currentBlock.x, chunkSize), PositiveModulo(currentBlock.y, chunkSize), PositiveModulo(currentBlock.z, chunkSize));
	chunkHitPosition = currentChunk;
	isHit = false;
	return false;
}

bool Physics::GetGrounded(Entity& entity, ChunkHandler& chunkHandler) {
	EntityData newEntityData = entity.GetEntityData();
	return CollideAxisFloat(1, newEntityData, chunkHandler) > 0;
}