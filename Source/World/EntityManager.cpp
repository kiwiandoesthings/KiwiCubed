#include "EntityManager.h"
#include "AssetManager.h"


EntityManager& EntityManager::GetInstance() {
    static EntityManager instance;
    return instance;
}

EntityManager::EntityManager() {
    return;
}

void EntityManager::RegisterEntity(EntityType entityType) {
    OVERRIDE_LOG_NAME("Entity Manager");
    stringIDsToEntityTypes.insert({entityType.entityStringID, entityType});

    INFO("Successfully registered entity type with string ID of \"" + entityType.entityStringID.CanonicalName() + "\"");
}

EntityType* EntityManager::GetEntityType(AssetStringID entityStringID) {
    OVERRIDE_LOG_NAME("Entity Manager");
    auto iterator = stringIDsToEntityTypes.find(entityStringID);
    if (iterator != stringIDsToEntityTypes.end()) {
        return &iterator->second;
    }
    CRITICAL("Tried to get entity type with string ID of \"" + entityStringID.CanonicalName() + "\" that didn't exist, aborting");
    psnip_trap();
    return nullptr;
}