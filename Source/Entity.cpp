#include "Entity.h"

Entity::Entity(float entityX, float entityY, float entityZ) : entityStats(EntityStats()), entityData(EntityData()) {
	entityData.position.x = entityX;
	entityData.position.y = entityY;
	entityData.position.z = entityZ;
}

EntityStats Entity::GetEntityStats() const {
	return entityStats;
}

EntityData Entity::GetEntityData() const {
	return entityData;
}

void Entity::SetEntityStats(EntityStats newEntityStats) {
	entityStats = newEntityStats;
}

void Entity::SetEntityData(EntityData newEntityData) {
	entityData = newEntityData;
}

void Entity::DamageEntity(float damage) {
	entityStats.health -= damage * (1 - static_cast<float>(entityStats.armor / 100));
	std::cout << damage * (1 - static_cast<float>(entityStats.armor / 100)) << std::endl;
}

void Entity::Update() {
	return;
}

void Entity::Render() {
	return;
}

void Entity::Delete() {

}