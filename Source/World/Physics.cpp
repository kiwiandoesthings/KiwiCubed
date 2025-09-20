#include "Physics.h"
#include "Entity.h"


static void ApplyGravity(EntityData& newEntityData) {
	Globals& globals = Globals::GetInstance();
	float gravityPerSecond = 9.81f;
    newEntityData.velocity.y -= gravityPerSecond * globals.deltaTime;
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

bool CollideAxis(unsigned char axis, EntityData& newEntityData, ChunkHandler& chunkHandler) {
	if (!newEntityData.currentChunkPtr->isGenerated) {
		return false;
	}

	glm::vec3 position = newEntityData.position;
	glm::ivec3 globalPosition = newEntityData.globalChunkPosition;
	int playerBlockX = FloorMod(static_cast<int>(position.x), 32);
	int playerBlockY = FloorMod(static_cast<int>(position.y), 32);
	int playerBlockZ = FloorMod(static_cast<int>(position.z), 32);
 	glm::vec3 min1 = glm::vec3(newEntityData.physicsBoundingBox.corner1.x + newEntityData.position.x, newEntityData.physicsBoundingBox.corner1.y + newEntityData.position.y, newEntityData.physicsBoundingBox.corner1.z + newEntityData.position.z);
	glm::vec3 max1 = glm::vec3(newEntityData.physicsBoundingBox.corner2.x + newEntityData.position.x, newEntityData.physicsBoundingBox.corner2.y + newEntityData.position.y, newEntityData.physicsBoundingBox.corner2.z + newEntityData.position.z);
	for (int x = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.x)); x <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.x)); ++x) {
		for (int y = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.y)); y <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.y)); ++y) {
			for (int z = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.z)); z <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.z)); ++z) {
				int targetChunkX = globalPosition.x + (playerBlockX + x < 0 ? -1 : playerBlockX + x >= 32 ? 1 : 0);
				int targetChunkY = globalPosition.y + (playerBlockY + y < 0 ? -1 : playerBlockY + y >= 32 ? 1 : 0);
				int targetChunkZ = globalPosition.z + (playerBlockZ + z < 0 ? -1 : playerBlockZ + z >= 32 ? 1 : 0);
				
				Chunk* targetChunk = nullptr;
				if (targetChunkX != globalPosition.x || targetChunkY != globalPosition.y || targetChunkZ != globalPosition.z) {
					targetChunk = &chunkHandler.GetChunk(targetChunkX, targetChunkY, targetChunkZ, false);
					if (!targetChunk->isGenerated) {
						continue;
					}
				} else {
					targetChunk = newEntityData.currentChunkPtr;
				}
	
				int localBlockX = PositiveModulo(playerBlockX + x, 32);
				int localBlockY = PositiveModulo(playerBlockY + y, 32);
				int localBlockZ = PositiveModulo(playerBlockZ + z, 32);
	
				Block& block = targetChunk->blocks[localBlockX][localBlockY][localBlockZ];
	
				if (!block.IsAir()) {
					glm::vec3 min2 = glm::vec3(localBlockX + (targetChunkX * chunkSize), localBlockY + (targetChunkY * chunkSize), localBlockZ + (targetChunkZ * chunkSize));
					std::cout << localBlockX << " " << targetChunkX << " " << targetChunkX * chunkSize << " " << (localBlockX + (targetChunkX * chunkSize)) << std::endl;
					glm::vec3 max2 = min2 + glm::vec3(1.0f);
	
					bool isColliding =
						(min1.x < max2.x && max1.x > min2.x) &&
						(min1.y < max2.y && max1.y > min2.y) &&
						(min1.z < max2.z && max1.z > min2.z);
	
					if (isColliding) {
						float dd = min1[axis] - min2[axis];
						if (dd < 0) {
							newEntityData.position[axis] = min2[axis] - newEntityData.physicsBoundingBox.corner2[axis];
						}
						else {
							newEntityData.position[axis] = max2[axis] - newEntityData.physicsBoundingBox.corner1[axis];
						}
						newEntityData.velocity[axis] = 0;
						return true;
					}
				}
			}
		}
	}
	return false;
}

float CollideAxisFloat(unsigned char axis, EntityData& newEntityData, ChunkHandler& chunkHandler) {
	if (!newEntityData.currentChunkPtr->isGenerated) {
		return 0.0f;
	}

	glm::vec3 position = newEntityData.position;
	glm::ivec3 globalPosition = newEntityData.globalChunkPosition;
	int playerBlockX = FloorMod(static_cast<int>(std::floor(position.x)), 32);
	int playerBlockY = FloorMod(static_cast<int>(std::floor(position.y)), 32);
	int playerBlockZ = FloorMod(static_cast<int>(std::floor(position.z)), 32);
	glm::vec3 min1 = glm::vec3(newEntityData.physicsBoundingBox.corner1.x + newEntityData.position.x, newEntityData.physicsBoundingBox.corner1.y + newEntityData.position.y, newEntityData.physicsBoundingBox.corner1.z + newEntityData.position.z);
	glm::vec3 max1 = glm::vec3(newEntityData.physicsBoundingBox.corner2.x + newEntityData.position.x, newEntityData.physicsBoundingBox.corner2.y + newEntityData.position.y, newEntityData.physicsBoundingBox.corner2.z + newEntityData.position.z);
	for (int x = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.x)); x <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.x)); ++x) {
		for (int y = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.y)); y <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.y)); ++y) {
			for (int z = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.z)); z <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.z)); ++z) {
				int targetChunkX = globalPosition.x + (playerBlockX + x < 0 ? -1 : playerBlockX + x >= 32 ? 1 : 0);
				int targetChunkY = globalPosition.y + (playerBlockY + y < 0 ? -1 : playerBlockY + y >= 32 ? 1 : 0);
				int targetChunkZ = globalPosition.z + (playerBlockZ + z < 0 ? -1 : playerBlockZ + z >= 32 ? 1 : 0);
	
				Chunk* targetChunk = nullptr;
				if (targetChunkX != globalPosition.x || targetChunkY != globalPosition.y || targetChunkZ != globalPosition.z) {
					targetChunk = &chunkHandler.GetChunk(targetChunkX, targetChunkY, targetChunkZ, false);
					if (!targetChunk->isGenerated) {
						continue;
					}
				} else {
					targetChunk = newEntityData.currentChunkPtr;
				}
	
				int localBlockX = PositiveModulo(playerBlockX + x, 32);
				int localBlockY = PositiveModulo(playerBlockY + y, 32);
				int localBlockZ = PositiveModulo(playerBlockZ + z, 32);
	
				Block& block = targetChunk->blocks[localBlockX][localBlockY][localBlockZ];
	
				if (!block.IsAir()) {
					glm::vec3 min2 = glm::vec3(localBlockX + (targetChunkX * chunkSize), localBlockY + (targetChunkY * chunkSize), localBlockZ + (targetChunkZ * chunkSize));
					glm::vec3 max2 = min2 + glm::vec3(1.0f);
	
					bool isColliding =
						(min1.x < max2.x && max1.x > min2.x) &&
						(min1.y < max2.y && max1.y > min2.y) &&
						(min1.z < max2.z && max1.z > min2.z);
	
					if (isColliding) {
						float dd = min1[axis] - min2[axis];
						if (dd < 0) {
							newEntityData.position[axis] = min2[axis] - newEntityData.physicsBoundingBox.corner2[axis];
						}
						else {
							newEntityData.position[axis] = max2[axis] - newEntityData.physicsBoundingBox.corner1[axis];
						}
						newEntityData.velocity[axis] = 0;
						return dd;
					}
				}
			}
		}
	}
	return 0.0f;
}

static bool ApplyTerrainCollision(EntityData& newEntityData, ChunkHandler& chunkHandler) {
	Globals& globals = Globals::GetInstance();
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

bool ApplyPhysics(Entity& entity, ChunkHandler& chunkHandler, bool applyGravity, bool applyCollision) {
	EntityData newEntityData = entity.GetEntityData();

	bool grounded = false;

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

	ClipVelocity(newEntityData, 0); 
	ClipVelocity(newEntityData, 1); 
	ClipVelocity(newEntityData, 2);

	if (!applyCollision) {
		newEntityData.velocity = glm::vec3(0);

		newEntityData.position.x += newEntityData.velocity.x;
		newEntityData.position.y += newEntityData.velocity.y;
		newEntityData.position.z += newEntityData.velocity.z;
	}

	entity.SetEntityData(newEntityData);
	return grounded;
}

// Literally what the fuck even is this functionn I hate raycasting so much
bool RaycastWorld(const glm::vec3& origin, const glm::vec3& direction, int maxDistance, ChunkHandler& chunkHandler, glm::ivec3& blockHitPosition, glm::ivec3& chunkHitPosition, bool& isHit) {
	glm::ivec3 currentBlock = glm::floor(origin);
	glm::ivec3 currentChunk = glm::ivec3(glm::floor(float(currentBlock.x) / float(chunkSize)), glm::floor(float(currentBlock.y) / float(chunkSize)),glm::floor(float(currentBlock.z) / float(chunkSize)));

	glm::ivec3 stepDirection = glm::ivec3(direction.x > 0 ? 1 : -1, direction.y > 0 ? 1 : -1, direction.z > 0 ? 1 : -1);

	glm::vec3 stepSize = glm::vec3(std::abs(1.0f / direction.x), std::abs(1.0f / direction.y), std::abs(1.0f / direction.z));
	glm::vec3 distanceToBlockBoundary = glm::vec3((currentBlock.x + (stepDirection.x > 0 ? 1 : 0) - origin.x) / direction.x, (currentBlock.y + (stepDirection.y > 0 ? 1 : 0) - origin.y) / direction.y, (currentBlock.z + (stepDirection.z > 0 ? 1 : 0) - origin.z) / direction.z);

	for (int i = 0; i < maxDistance; ++i) {
		glm::ivec3 localBlockPos = glm::ivec3(PositiveModulo(currentBlock.x, chunkSize), PositiveModulo(currentBlock.y, chunkSize), PositiveModulo(currentBlock.z, chunkSize));

		Chunk chunk = chunkHandler.GetChunk(currentChunk.x, currentChunk.y, currentChunk.z, false);
		if (chunk.isGenerated) {
			Block& block = chunk.blocks[localBlockPos.x][localBlockPos.y][localBlockPos.z];
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

bool GetGrounded(Entity& entity, ChunkHandler& chunkHandler) {
	EntityData newEntityData = entity.GetEntityData();
	return CollideAxisFloat(1, newEntityData, chunkHandler) > 0;
}