#include "Physics.h"
#include "Entity.h"


void* operator new(size_t size) {
	//std::cout << "allocated " << size << " bytes" << std::endl;
	return malloc(size);
}


static void ApplyGravity(EntityData& newEntityData) {
	if (abs(newEntityData.velocity.y) < newEntityData.terminalVelocity) {
		newEntityData.velocity.y -= 0.00918f;
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

static int positiveModulo(int a, int b) {
	return (a % b + b) % b;
}

bool CollideAxis(short axis, EntityData& newEntityData, ChunkHandler& chunkHandler, bool actuallyCollide) {
	int playerChunkX = static_cast<int>(newEntityData.position.x / 32);
	int playerChunkY = static_cast<int>(newEntityData.position.y / 32);
	int playerChunkZ = static_cast<int>(newEntityData.position.z / 32);

	if (!chunkHandler.GetChunk(playerChunkX, playerChunkY, playerChunkZ, false).isGenerated) {
		newEntityData.position += newEntityData.velocity;
		newEntityData.velocity = glm::vec3(0, 0, 0);
		return false;
	}
	if (axis == 0) {
		newEntityData.position.x += newEntityData.velocity.x;

		if (!actuallyCollide) {
			return false;
		}

		glm::vec3 position = newEntityData.position;
		int playerChunkX = static_cast<int>(position.x / 32);
		int playerChunkY = static_cast<int>(position.y / 32);
		int playerChunkZ = static_cast<int>(position.z / 32);
		int playerBlockX = positiveModulo(static_cast<int>(position.x), 32);
		int playerBlockY = positiveModulo(static_cast<int>(position.y), 32);
		int playerBlockZ = positiveModulo(static_cast<int>(position.z), 32);
		glm::vec3 min1 = glm::vec3(newEntityData.physicsBoundingBox.corner1.x + newEntityData.position.x, newEntityData.physicsBoundingBox.corner1.y + newEntityData.position.y, newEntityData.physicsBoundingBox.corner1.z + newEntityData.position.z);
		glm::vec3 max1 = glm::vec3(newEntityData.physicsBoundingBox.corner2.x + newEntityData.position.x, newEntityData.physicsBoundingBox.corner2.y + newEntityData.position.y, newEntityData.physicsBoundingBox.corner2.z + newEntityData.position.z);
		for (int x = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.x)); x <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.x)); ++x) {
			for (int y = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.y)); y <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.y)); ++y) {
				for (int z = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.z)); z <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.z)); ++z) {
					int targetChunkX = playerChunkX + (playerBlockX + x < 0 ? -1 : playerBlockX + x >= 32 ? 1 : 0);
					int targetChunkY = playerChunkY + (playerBlockY + y < 0 ? -1 : playerBlockY + y >= 32 ? 1 : 0);
					int targetChunkZ = playerChunkZ + (playerBlockZ + z < 0 ? -1 : playerBlockZ + z >= 32 ? 1 : 0);
		
					Chunk& targetChunk = chunkHandler.GetChunk(targetChunkX, targetChunkY, targetChunkZ, false);
		
					if (!targetChunk.isGenerated) {
						continue;
					}
		
					int localBlockX = positiveModulo(playerBlockX + x, 32);
					int localBlockY = positiveModulo(playerBlockY + y, 32);
					int localBlockZ = positiveModulo(playerBlockZ + z, 32);
		
					Block& block = targetChunk.blocks[localBlockX][localBlockY][localBlockZ];
		
					if (!block.IsAir() > 0) {
						glm::vec3 min2 = glm::vec3(localBlockX + (targetChunkX * chunkSize), localBlockY + (targetChunkY * chunkSize), localBlockZ + (targetChunkZ * chunkSize));
						glm::vec3 max2 = min2 + glm::vec3(1.0f);
		
						bool isColliding =
							(min1.x < max2.x && max1.x > min2.x) &&
							(min1.y < max2.y && max1.y > min2.y) &&
							(min1.z < max2.z && max1.z > min2.z);
		
						float dx = min1.x - min2.x;
		
						if (isColliding) {
							if (dx < 0) {
								newEntityData.position.x = min2.x - newEntityData.physicsBoundingBox.corner2.x;
							}
							else {
								newEntityData.position.x = max2.x - newEntityData.physicsBoundingBox.corner1.x;
							}
							newEntityData.velocity.x = 0;
							return true;
						}
					}
				}
			}
		}
		return false;
	}
	else if (axis == 1) {
		newEntityData.position.y += newEntityData.velocity.y;

		if (!actuallyCollide) {
			return false;
		}

		glm::vec3 position = newEntityData.position;
		int playerChunkX = static_cast<int>(position.x / 32);
		int playerChunkY = static_cast<int>(position.y / 32);
		int playerChunkZ = static_cast<int>(position.z / 32);
		int playerBlockX = positiveModulo(static_cast<int>(position.x), 32);
		int playerBlockY = positiveModulo(static_cast<int>(position.y), 32);
		int playerBlockZ = positiveModulo(static_cast<int>(position.z), 32);
		glm::vec3 min1 = glm::vec3(newEntityData.physicsBoundingBox.corner1.x + newEntityData.position.x, newEntityData.physicsBoundingBox.corner1.y + newEntityData.position.y, newEntityData.physicsBoundingBox.corner1.z + newEntityData.position.z);
		glm::vec3 max1 = glm::vec3(newEntityData.physicsBoundingBox.corner2.x + newEntityData.position.x, newEntityData.physicsBoundingBox.corner2.y + newEntityData.position.y, newEntityData.physicsBoundingBox.corner2.z + newEntityData.position.z);
		for (int x = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.x)); x <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.x)); ++x) {
			for (int y = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.y)); y <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.y)); ++y) {
				for (int z = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.z)); z <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.z)); ++z) {
					int targetChunkX = playerChunkX + (playerBlockX + x < 0 ? -1 : playerBlockX + x >= 32 ? 1 : 0);
					int targetChunkY = playerChunkY + (playerBlockY + y < 0 ? -1 : playerBlockY + y >= 32 ? 1 : 0);
					int targetChunkZ = playerChunkZ + (playerBlockZ + z < 0 ? -1 : playerBlockZ + z >= 32 ? 1 : 0);
		
					Chunk& targetChunk = chunkHandler.GetChunk(targetChunkX, targetChunkY, targetChunkZ, false);
		
					if (!targetChunk.isGenerated) {
						continue;
					}
		
					int localBlockX = positiveModulo(playerBlockX + x, 32);
					int localBlockY = positiveModulo(playerBlockY + y, 32);
					int localBlockZ = positiveModulo(playerBlockZ + z, 32);
		
					Block& block = targetChunk.blocks[localBlockX][localBlockY][localBlockZ];
		
					if (!block.IsAir() > 0) {
						glm::vec3 min2 = glm::vec3(localBlockX + (targetChunkX * chunkSize), localBlockY + (targetChunkY * chunkSize), localBlockZ + (targetChunkZ * chunkSize));
						glm::vec3 max2 = min2 + glm::vec3(1.0f);
		
						bool isColliding =
							(min1.x < max2.x && max1.x > min2.x) &&
							(min1.y < max2.y && max1.y > min2.y) &&
							(min1.z < max2.z && max1.z > min2.z);
		
						float dy = min1.y - min2.y;
		
						if (isColliding) {
							if (dy < 0) {
								newEntityData.position.y = min2.y - newEntityData.physicsBoundingBox.corner2.y;
							}
							else {
								newEntityData.position.y = max2.y - newEntityData.physicsBoundingBox.corner1.y;
							}
							newEntityData.velocity.y = 0;
							return true;
						}
					}
				}
			}
		}
		return false;
	}
	else if (axis == 2) {
		newEntityData.position.z += newEntityData.velocity.z;

		if (!actuallyCollide) {
			return false;
		}

		glm::vec3 position = newEntityData.position;
		int playerBlockX = positiveModulo(static_cast<int>(position.x), 32);
		int playerBlockY = positiveModulo(static_cast<int>(position.y), 32);
		int playerBlockZ = positiveModulo(static_cast<int>(position.z), 32);
		glm::vec3 min1 = glm::vec3(newEntityData.physicsBoundingBox.corner1.x + newEntityData.position.x, newEntityData.physicsBoundingBox.corner1.y + newEntityData.position.y, newEntityData.physicsBoundingBox.corner1.z + newEntityData.position.z);
		glm::vec3 max1 = glm::vec3(newEntityData.physicsBoundingBox.corner2.x + newEntityData.position.x, newEntityData.physicsBoundingBox.corner2.y + newEntityData.position.y, newEntityData.physicsBoundingBox.corner2.z + newEntityData.position.z);
		for (int x = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.x)); x <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.x)); ++x) {
			for (int y = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.y)); y <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.y)); ++y) {
				for (int z = static_cast<int>(std::floor(newEntityData.physicsBoundingBox.corner1.z)); z <= static_cast<int>(std::ceil(newEntityData.physicsBoundingBox.corner2.z)); ++z) {
					int targetChunkX = playerChunkX + (playerBlockX + x < 0 ? -1 : playerBlockX + x >= 32 ? 1 : 0);
					int targetChunkY = playerChunkY + (playerBlockY + y < 0 ? -1 : playerBlockY + y >= 32 ? 1 : 0);
					int targetChunkZ = playerChunkZ + (playerBlockZ + z < 0 ? -1 : playerBlockZ + z >= 32 ? 1 : 0);
		
					Chunk& targetChunk = chunkHandler.GetChunk(targetChunkX, targetChunkY, targetChunkZ, false);
		
					if (!targetChunk.isGenerated) {
						continue;
					}
		
					int localBlockX = positiveModulo(playerBlockX + x, 32);
					int localBlockY = positiveModulo(playerBlockY + y, 32);
					int localBlockZ = positiveModulo(playerBlockZ + z, 32);
		
					Block& block = targetChunk.blocks[localBlockX][localBlockY][localBlockZ];
		
					if (!block.IsAir() > 0) {
						glm::vec3 min2 = glm::vec3(localBlockX + (targetChunkX * chunkSize), localBlockY + (targetChunkY * chunkSize), localBlockZ + (targetChunkZ * chunkSize));
						glm::vec3 max2 = min2 + glm::vec3(1.0f);
		
						bool isColliding =
							(min1.x < max2.x && max1.x > min2.x) &&
							(min1.y < max2.y && max1.y > min2.y) &&
							(min1.z < max2.z && max1.z > min2.z);
		
						float dz = min1.z - min2.z;
		
						if (isColliding) {
							if (dz < 0) {
								newEntityData.position.z = min2.z - newEntityData.physicsBoundingBox.corner2.z;
							}
							else {
								newEntityData.position.z = max2.z - newEntityData.physicsBoundingBox.corner1.z;
							}
							newEntityData.velocity.z = 0;
							return true;
						}
					}
				}
			}
		}
		return false;
	}
	return false;
}

static bool ApplyTerrainCollision(EntityData& newEntityData, ChunkHandler& chunkHandler, bool actuallyCollide = true) {
	bool xAxis = CollideAxis(0, newEntityData, chunkHandler, actuallyCollide);
	bool yAxis = CollideAxis(1, newEntityData, chunkHandler, actuallyCollide);
	bool zAxis = CollideAxis(2, newEntityData, chunkHandler, actuallyCollide);

	newEntityData.globalChunkPosition = glm::ivec3(
		(newEntityData.position.x >= 0)
		? static_cast<int>(newEntityData.position.x / 32)
		: static_cast<int>((newEntityData.position.x - 31) / 32),
		(newEntityData.position.y >= 0)
		? static_cast<int>(newEntityData.position.y / 32)
		: static_cast<int>((newEntityData.position.y - 31) / 32),
		(newEntityData.position.z >= 0)
		? static_cast<int>(newEntityData.position.z / 32)
		: static_cast<int>((newEntityData.position.z - 31) / 32)
	);

	newEntityData.localChunkPosition = glm::ivec3(positiveModulo(static_cast<int>(newEntityData.position.x), 32), positiveModulo(static_cast<int>(newEntityData.position.y), 32) , positiveModulo(static_cast<int>(newEntityData.position.z), 32));

	newEntityData.velocity = glm::vec3(0, 0, 0);

	if (xAxis || yAxis || zAxis) {
		return true;
	}

	return false;
}

void ApplyPhysics(Entity& entity, ChunkHandler& chunkHandler, bool applyGravity, bool applyCollision) {
	EntityData newEntityData = entity.GetEntityData();

	if (applyGravity) {
		ApplyGravity(newEntityData);
	}
	ApplyTerrainCollision(newEntityData, chunkHandler, applyCollision);


	entity.SetEntityData(newEntityData);
}

// Literally what the fuck even is this functionn I hate raycasting so much
bool RaycastWorld(const glm::vec3& origin, const glm::vec3& direction, int maxDistance, ChunkHandler& chunkHandler, glm::ivec3& blockHitPosition, glm::ivec3& chunkHitPosition, bool& isHit) {
	glm::ivec3 currentBlock = glm::floor(origin );
	glm::ivec3 currentChunk = glm::ivec3(glm::floor(float(currentBlock.x) / float(chunkSize)), glm::floor(float(currentBlock.y) / float(chunkSize)),glm::floor(float(currentBlock.z) / float(chunkSize)));

	glm::ivec3 stepDirection = glm::ivec3(direction.x > 0 ? 1 : -1, direction.y > 0 ? 1 : -1, direction.z > 0 ? 1 : -1);

	glm::vec3 stepSize = glm::vec3(std::abs(1.0f / direction.x), std::abs(1.0f / direction.y), std::abs(1.0f / direction.z));
	glm::vec3 distanceToBlockBoundary = glm::vec3((currentBlock.x + (stepDirection.x > 0 ? 1 : 0) - origin.x) / direction.x, (currentBlock.y + (stepDirection.y > 0 ? 1 : 0) - origin.y) / direction.y, (currentBlock.z + (stepDirection.z > 0 ? 1 : 0) - origin.z) / direction.z);

	for (int i = 0; i < maxDistance; ++i) {
		glm::ivec3 localBlockPos = glm::ivec3(positiveModulo(currentBlock.x, chunkSize), positiveModulo(currentBlock.y, chunkSize), positiveModulo(currentBlock.z, chunkSize));

		Chunk chunk = chunkHandler.GetChunk(currentChunk.x, currentChunk.y, currentChunk.z, false);
		if (chunk.isGenerated) {
			Block& block = chunk.blocks[localBlockPos.x][localBlockPos.y][localBlockPos.z];
			if (!block.IsAir()) {
				blockHitPosition = glm::ivec3(positiveModulo(currentBlock.x, chunkSize), positiveModulo(currentBlock.y, chunkSize), positiveModulo(currentBlock.z, chunkSize));
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

	blockHitPosition = glm::ivec3(positiveModulo(currentBlock.x, chunkSize), positiveModulo(currentBlock.y, chunkSize), positiveModulo(currentBlock.z, chunkSize));
	chunkHitPosition = currentChunk;
	isHit = false;
	return false;
}