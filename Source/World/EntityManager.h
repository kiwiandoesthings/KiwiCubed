#pragma once

#include "AssetManager.h"
#include "Entity.h"
#include "ModHandler.h"


class EntityManager {
    public:
        static EntityManager& GetInstance();

        EntityManager();

        void RegisterEntity(EntityType entity);

        EntityType* GetEntityType(AssetStringID entityStringID);

    private:
        ~EntityManager() = default;

        EntityManager(const EntityManager&) = delete;
        EntityManager& operator=(const EntityManager&) = delete;

        std::unordered_map<AssetStringID, EntityType> stringIDsToEntityTypes;
};