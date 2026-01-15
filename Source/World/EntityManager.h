#pragma once

#include "AssetManager.h"
#include "Entity.h"
#include "ModHandler.h"


class EntityManager {
    public:
        static EntityManager& GetInstance();

        EntityManager();

        void RegisterEntityType(EntityType entityType);
        EntityType* GetEntityType(AssetStringID entityStringID);

        void SpawnEntity(AssetStringID entityTypeString, glm::vec3 position);

        std::vector<Entity*> GetAllEntities() const;
        std::vector<Entity*> GetEntitesOfType(AssetStringID entityTypeString) const;

        template<typename Function>
        void ForEachEntity(Function&& function) {
            for (auto& entity : entities) {
                function(*entity.get());
            } 
        }

        void EmitEvent(std::string eventName);

    private:
        ~EntityManager() = default;

        EntityManager(const EntityManager&) = delete;
        EntityManager& operator=(const EntityManager&) = delete;

        std::unordered_map<AssetStringID, EntityType> stringIDsToEntityTypes;
        std::vector<std::unique_ptr<Entity>> entities;
        std::unordered_map<AssetStringID, std::vector<int>> entityTypesToEntities;
};

inline EntityManager& entityManager = EntityManager::GetInstance();