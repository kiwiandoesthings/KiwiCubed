#include "Entity.h"
#include "World.h"

Entity::Entity(float entityX, float entityY, float entityZ, World& world) : world(world), entityStats(EntityStats()), entityData(EntityData()) {
	entityData.position.x = entityX;
	entityData.position.y = entityY;
	entityData.position.z = entityZ;

	protectedEntityData.UUID = CreateUUID().c_str();
}

EntityStats Entity::GetEntityStats() const {
	return entityStats;
}

EntityData Entity::GetEntityData() const {
	return entityData;
}

ProtectedEntityData Entity::GetProtectedEntityData() const {
	return protectedEntityData;
}

void Entity::SetEntityStats(EntityStats newEntityStats) {
	entityStats = newEntityStats;
}

void Entity::SetEntityData(EntityData newEntityData) {
	entityData = newEntityData;
}

void Entity::DamageEntity(float damage) {
	entityStats.health -= damage * (1 - static_cast<float>(static_cast<float>(entityStats.armor) / 100));
}

void Entity::Update() {
	glm::ivec3 oldGlobalChunkPosition = entityData.globalChunkPosition;
	entityData.globalChunkPosition = glm::ivec3(
		Physics::FloorDiv(entityData.position.x, chunkSize),
		Physics::FloorDiv(entityData.position.y, chunkSize),
		Physics::FloorDiv(entityData.position.z, chunkSize)
	);
	entityData.localChunkPosition = glm::ivec3(
		Physics::PositiveModulo(entityData.position.x, chunkSize), 
		Physics::PositiveModulo(entityData.position.y, chunkSize) , 
		Physics::PositiveModulo(entityData.position.z, chunkSize)
	);

	if (oldGlobalChunkPosition != entityData.globalChunkPosition) {
		entityData.currentChunkPtr = &world.GetChunkHandler().GetChunk(entityData.globalChunkPosition.x, entityData.globalChunkPosition.y, entityData.globalChunkPosition.z, false);
	}

	return;
}

void Entity::Render() {
	return;
}

std::string Entity::CreateUUID() {
	std::array<uint8_t, 16> uuidBytes;

	std::random_device random;
	std::mt19937 gen(random());
	std::uniform_int_distribution<unsigned int> dist(0, 255);

	for (auto &byte : uuidBytes) {
		byte = dist(gen);
	}
	
	uuidBytes[6] = (uuidBytes[6] & 0x0F) | 0x40;
	uuidBytes[8] = (uuidBytes[8] & 0x3F) | 0x80;

	std::ostringstream stringStream;
	stringStream << std::hex << std::setfill('0');
	for (size_t i = 0; i < uuidBytes.size(); i++) {
		stringStream << std::setw(2) << static_cast<unsigned int>(uuidBytes[i]);
		if (i == 3 || i == 5 || i == 7 || i == 9) {
			stringStream << "-";
		}
	}
	
	return stringStream.str();
}

void Entity::Delete() {

}