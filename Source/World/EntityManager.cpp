#include "EntityManager.h"
#include "AssetManager.h"


EntityManager& EntityManager::GetInstance() {
    static EntityManager instance;
    return instance;
}

EntityManager::EntityManager() {
    return;
}

void EntityManager::Setup(World* world) {
	this->world = world;
}

void EntityManager::RegisterEntityType(EntityType entityType) {
    OVERRIDE_LOG_NAME("Entity Manager");
    stringIDsToEntityTypes.insert({entityType.entityStringID, entityType});

    INFO("Successfully registered entity type with string ID of \"" + entityType.entityStringID.CanonicalName() + "\"");
}

EntityType* EntityManager::GetEntityType(AssetStringID entityTypeString) {
    OVERRIDE_LOG_NAME("Entity Manager");
    auto iterator = stringIDsToEntityTypes.find(entityTypeString);
    if (iterator != stringIDsToEntityTypes.end()) {
        return &iterator->second;
    }
    CRITICAL("Tried to get entity type with string ID of \"" + entityTypeString.CanonicalName() + "\" that didn't exist, aborting");
    psnip_trap();
    return nullptr;
}

siv::id EntityManager::SpawnEntity(AssetStringID entityTypeString, AssetStringID modelID, AssetStringID atlasID, AssetStringID textureID, glm::vec3 position) {
    EntityType* entityType = GetEntityType(entityTypeString);

	siv::id entityID = entities.emplace_back(position.x, position.y, position.z, world);
	entityTypesToEntities[entityTypeString].push_back(entityID);

	entities[entityID].SetupRenderComponents(modelID, atlasID, textureID);

	std::cout << "created entity with type " << entityTypeString.CanonicalName() << std::endl;

	return entityID;
}

siv::vector<Entity> EntityManager::GetAllEntities() const {
    return entities;
}

std::vector<siv::id> EntityManager::GetEntitesOfType(AssetStringID entityTypeString) const {
    std::vector<siv::id> existentEntities;
    auto entitiesOfType = entityTypesToEntities.find(entityTypeString);
    if (entitiesOfType != entityTypesToEntities.end()) {
        existentEntities.reserve(entitiesOfType->second.size());
        for (auto& index : entitiesOfType->second) {
            existentEntities.push_back(index);
        }
        return existentEntities;
    } else {
        return std::vector<siv::id>();
    }
}