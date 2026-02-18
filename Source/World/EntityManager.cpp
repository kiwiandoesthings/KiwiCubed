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

siv::handle<Entity*> EntityManager::SpawnPlayer(glm::vec3 position) {
	std::lock_guard<std::mutex> lock(entitiesMutex);
	siv::id nextID = entities.getNextID();
	Player* player = new Player(static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.z), world, nextID, GetEntityType(AssetStringID{"kiwicubed", "entity/player"}));
	siv::id entityID = entities.emplace_back(player);
	entityTypesToEntities[AssetStringID{"kiwicubed", "entity/player"}].push_back(entityID);

	return entities.createhandle(entityID);
}

siv::handle<Entity*> EntityManager::SpawnEntity(AssetStringID entityTypeString, glm::vec3 position) {
   	std::lock_guard<std::mutex> lock(entitiesMutex);
	EntityType* entityType = GetEntityType(entityTypeString);

	siv::id nextID = entities.getNextID();
	Entity* entity = new Entity(position.x, position.y, position.z, world, nextID, entityType);
	siv::id entityID = entities.emplace_back(entity);
	entityTypesToEntities[entityTypeString].push_back(entityID);

	return entities.createhandle(entityID);
}

siv::handle<Entity*> EntityManager::SpawnEntity(AssetStringID entityTypeString, AssetStringID modelID, AssetStringID atlasID, AssetStringID textureID, glm::vec3 position) {
    std::lock_guard<std::mutex> lock(entitiesMutex);
	EntityType* entityType = GetEntityType(entityTypeString);

	siv::id nextID = entities.getNextID();
	Entity* entity = new Entity(position.x, position.y, position.z, world, nextID, entityType);
	siv::id entityID = entities.emplace_back(entity);
	entityTypesToEntities[entityTypeString].push_back(entityID);

	entities[entityID]->SetupRenderComponents(modelID, atlasID, textureID);

	return entities.createhandle(entityID);
}

siv::id EntityManager::RemoveEntity(siv::id entityID) {
	std::lock_guard<std::mutex> lock(entitiesMutex);
	if (!entities.isValidID(entityID)) {
		CRITICAL("Tried to remove entity with id {" + std::to_string(entityID) + "} that didn't exist, aborting");
		psnip_trap();
	}
	auto entitiesOfType = entityTypesToEntities.find(entities[entityID]->GetEntityType()->entityStringID);
	if (entitiesOfType == entityTypesToEntities.end()) {
		CRITICAL("Tried to remove entity with id {" + std::to_string(entityID) + "} that didn't exist, aborting (This should never happen, please report a bug)");
		psnip_trap();
	}
	std::vector<uint64_t>& entityIDs = entitiesOfType->second;
	size_t originalLength = entityIDs.size();
	entityIDs.erase(std::remove(entityIDs.begin(), entityIDs.end(), entityID), entityIDs.end());
	if (originalLength - 1 != entityIDs.size()) {
		CRITICAL("Length mismatch after removing entityID from entityTypesToEntities vector, length after removing is {" + std::to_string(entityIDs.size()) + "} when it should have been {" + std::to_string(originalLength - 1) + "}");
	}
	entities.erase(entityID);
	return entities.getNextID();
}

Entity* EntityManager::GetEntity(const uint64_t entityID) {
	std::lock_guard<std::mutex> lock(entitiesMutex);
	return entities[entityID];
}

siv::vector<Entity*> EntityManager::GetAllEntities() {
    std::lock_guard<std::mutex> lock(entitiesMutex);
	return entities;
}

std::vector<siv::id> EntityManager::GetEntitesOfType(AssetStringID entityTypeString) {
    std::lock_guard<std::mutex> lock(entitiesMutex);
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

std::mutex& EntityManager::GetEntitiesMutex() {
	return entitiesMutex;
}