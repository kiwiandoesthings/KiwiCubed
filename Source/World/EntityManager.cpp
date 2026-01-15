#include "EntityManager.h"
#include "AssetManager.h"


EntityManager& EntityManager::GetInstance() {
    static EntityManager instance;
    return instance;
}

EntityManager::EntityManager() {
    return;
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

void EntityManager::SpawnEntity(AssetStringID entityTypeString, glm::vec3 position) {
    EntityType* entityType = GetEntityType(entityTypeString);
}

std::vector<Entity*> EntityManager::GetAllEntities() const {
    std::vector<Entity*> existentEntities;
    for (auto& entity : entities) {
        if (entity) {
            existentEntities.push_back(entity.get());
        }
    }
    return existentEntities;
}

std::vector<Entity*> EntityManager::GetEntitesOfType(AssetStringID entityTypeString) const {
    std::vector<Entity*> existentEntities;
    auto entitiesOfType = entityTypesToEntities.find(entityTypeString);
    if (entitiesOfType != entityTypesToEntities.end()) {
        existentEntities.reserve(entitiesOfType->second.size());
        for (auto& index : entitiesOfType->second) {
            if (entities[index]) {
                existentEntities.push_back(entities[index].get());
            }
        }
        return existentEntities;
    } else {
        return std::vector<Entity*>();
    }
}

void EntityManager::EmitEvent(std::string eventName) {
    for (const auto& typesToEntities : entityTypesToEntities) {
        EntityType* entityType = GetEntityType(typesToEntities.first);
        
        bool found = false;
        std::string callback;
        for (const auto& eventsToCallbacks : entityType->eventsToCallbacks) {
            if (eventsToCallbacks.first == eventName) {
                found = true;
                callback = eventsToCallbacks.second;
            }
        }

        if (!found) {
            continue;
        }

        for (auto& entity : typesToEntities.second) {
            
        }
    }
}