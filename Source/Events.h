#pragma once

#include <klogger.hpp>

#include <algorithm>
#include <any>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include <angelscript.h>

#include "EntityManager.h"
#include "EventDefinitions.h"


const inline unsigned long long VerificationNumber = 0x4B49574943554245ULL;


enum EventType {
	EVENT_INVALID = -1,

    // Meta events
    EVENT_META_WINDOW_MINIMIZE,
    EVENT_META_WINDOW_MAXIMIZE,
    EVENT_META_WINDOW_RESIZE,
    // World events
    //  Important events
    EVENT_WORLD_TICK,
    //  Player events
    EVENT_WORLD_PLAYER_BLOCK,
    EVENT_WORLD_PLAYER_MOVE,
	//  Entity events
	EVENT_WORLD_ENTITY_BLOCK,
	EVENT_WORLD_ENTITY_HURT,
	EVENT_WORLD_ENTITY_ATTACK,
	//  Generic events
    EVENT_WORLD_GENERIC_BLOCK,
};


template<EventType T> struct EventTypesToStruct;
template<> struct EventTypesToStruct<EVENT_META_WINDOW_RESIZE> {
	using Type = EventMetaWindowResize;
};
template<> struct EventTypesToStruct<EVENT_WORLD_TICK> {
	using Type = EventWorldTick;
};
template<> struct EventTypesToStruct<EVENT_WORLD_PLAYER_BLOCK> {
	using Type = EventWorldPlayerBlock;
};
template<> struct EventTypesToStruct<EVENT_WORLD_PLAYER_MOVE> {
	using Type = EventWorldPlayerMove;
};
template<> struct EventTypesToStruct<EVENT_WORLD_ENTITY_BLOCK> {
	using Type = EventWorldEntityBlock;
};
template<> struct EventTypesToStruct<EVENT_WORLD_ENTITY_HURT> {
	using Type = EventWorldEntityHurt;
};
template<> struct EventTypesToStruct<EVENT_WORLD_ENTITY_ATTACK> {
	using Type = EventWorldEntityAttack;
};
template<> struct EventTypesToStruct<EVENT_WORLD_GENERIC_BLOCK> {
	using Type = EventWorldGenericBlock;
};

const std::string eventTypeStrings[10] = {
	"EventMetaWindowMinimize",
	"EventMetaWindowMaximize",
	"EventMetaWindowResize",
	"EventWorldTick",
	"EventWorldPlayerBlock",
	"EventWorldPlayerMove",
	"EventWorldEntityBlock",
	"EventWorldEntityHurt",
	"EventWorldEntityAttack",
	"EventWorldGenericBlock"
};

struct EventData {
	EventType type = EVENT_INVALID;
	void* data = nullptr;
	size_t dataSize = 0;
	const unsigned long long verification = VerificationNumber;

	template<typename T>
	const T* GetDataStruct() const {
		OVERRIDE_LOG_NAME("Event Data");

		if (verification != VerificationNumber) {
			ERR("EventData was corruped somewhere, verification number did not match");
			psnip_trap();
		}
		if (sizeof(T) != dataSize) {
			ERR("Tried to get data struct from EventData where data struct's size and EventData's dataSize were unequal");
			psnip_trap();
		}

		return static_cast<const T*>(data);
	}

	template<typename T>
	static const EventData GetEventData(T customData, EventType dataType) {
		return EventData{dataType, &customData, sizeof(customData)};
	}

	EventData& operator=(const EventData& other) {
		if (this == &other) {
			return *this;
		}

		free(data);

		type = other.type;
		dataSize = other.dataSize;

		data = malloc(other.dataSize);
		memcpy(data, other.data, other.dataSize);

		return *this;
	}

	EventData(const EventData& other) {
		type = other.type;
		dataSize = other.dataSize;

		data = malloc(other.dataSize);
		memcpy(data, other.data, other.dataSize);
	}

	EventData(EventType eventType, void* data, size_t dataSize) : type(eventType), dataSize(dataSize) {
		OVERRIDE_LOG_NAME("Event Data");
		if (data && dataSize > 0) {
			this->data = malloc(dataSize);
			memcpy(this->data, data, dataSize);
		} else {
			if (!data) {
				ERR("EventData created with invalid data pointer");
			} else {
				ERR("EventData created with size of 0");
			}
			psnip_trap();
		}
	}
	~EventData() {
		if (data != nullptr) {
			free(data);
			type = EVENT_INVALID;
			data = nullptr;
			dataSize = 0;
		}
	}
};


class EventManager {
    public:
        static EventManager& GetInstance();
        static void Delete();

		static bool RegisterEventToEntityType(EventType eventType, const AssetStringID& entityTypeString);
		static void RegisterFunctionToEvent(EventType eventType, std::function<void(const EventData&)> callback);
		static void RegisterScriptToEvent(EventType eventType, const std::string& moduleName);

        static bool TriggerEvent(EventType eventType, const EventData eventData);

    private:
        EventManager() = default;
        ~EventManager() = default;

        EventManager(const EventManager&) = delete;
        EventManager& operator=(const EventManager&) = delete;

		static std::unordered_map<EventType, std::vector<AssetStringID>> eventsToEntityTypes;
		static std::unordered_map<EventType, std::vector<std::function<void(EventData&)>>> eventsToFunctions;
		static std::unordered_map<EventType, std::vector<std::string>> eventsToScripts;

		static std::vector<uint8_t> eventDatas;
};