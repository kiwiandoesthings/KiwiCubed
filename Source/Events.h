#pragma once

#include <klogger.hpp>

#include <algorithm>
#include <any>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "EntityManager.h"
#include "EventDefinitions.h"


const inline unsigned long long VerificationNumber = 0x4B49574943554245ULL;


enum EventType {
    // Meta events
    EVENT_META_WINDOW_MINIMIZE,
    EVENT_META_WINDOW_MAXIMIZE,
    EVENT_META_WINDOW_RESIZE,
    // World events
    //  Important events
    EVENT_WORLD_TICK,
    //  Player events
    EVENT_WORLD_PLAYER_BLOCK_EVENT,
    EVENT_WORLD_PLAYER_MOVE,
	//  Entity events
	EVENT_WORLD_ENTITY_BLOCK_EVENT,
	EVENT_WORLD_ENTITY_HURT_EVENT,
	EVENT_WORLD_ENTITY_ATTACK_EVENT,
	//  Generic events
    EVENT_WORLD_GENERIC_BLOCK_EVENT,
};

struct EventData {
	void* data;
	size_t dataSize;
	unsigned long long verification = VerificationNumber;

	EventData(void* data, size_t dataSize) : data(data), dataSize(dataSize) {}
};


class EventManager {
    public:
        static EventManager& GetInstance();
        void Delete();

		bool RegisterEventToEntityType(EventType eventType, AssetStringID entityTypeString);
		void RegisterFunctionToEvent(EventType eventType, std::function<void(EventData&)> callback);

        bool TriggerEvent(EventType eventType, EventData eventData);

    private:
        EventManager() = default;
        ~EventManager() = default;

        EventManager(const EventManager&) = delete;
        EventManager& operator=(const EventManager&) = delete;

		std::unordered_map<EventType, std::vector<AssetStringID>> eventsToEntityTypes;
		std::unordered_map<EventType, std::vector<std::function<void(EventData&)>>> eventsToFunctions;

		std::vector<uint8_t> eventDatas;
};