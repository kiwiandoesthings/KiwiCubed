#pragma once

#include <index_vector.hpp>

#include "AssetManager.h"
#include "Entity.h"


class EntityManager {
    public:
        static EntityManager& GetInstance();

		EntityManager();
		
        void Setup(World* world);

        void RegisterEntityType(EntityType entityType);
        EntityType* GetEntityType(AssetStringID entityStringID);

        siv::id SpawnEntity(AssetStringID entityTypeString, AssetStringID modelID, AssetStringID atlasID, AssetStringID textureID, glm::vec3 position);

		Entity& GetEntity(const uint64_t entityID);
        siv::vector<Entity> GetAllEntities() const;
        std::vector<siv::id> GetEntitesOfType(AssetStringID entityTypeString) const;

        template<typename Function>
        void ForEachEntity(Function&& function) {
            for (auto& entity : entities) {
                function(entity);
            } 
        }

		std::mutex& GetEntitiesMutex();

    private:
        ~EntityManager() = default;

        EntityManager(const EntityManager&) = delete;
        EntityManager& operator=(const EntityManager&) = delete;

		World* world = nullptr;

		std::mutex entitiesMutex;
        std::unordered_map<AssetStringID, EntityType> stringIDsToEntityTypes;
		siv::vector<Entity> entities;
        std::unordered_map<AssetStringID, std::vector<siv::id>> entityTypesToEntities;
};

inline EntityManager& entityManager = EntityManager::GetInstance();