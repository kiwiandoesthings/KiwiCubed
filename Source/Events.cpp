#include "Events.h"

EventManager& EventManager::GetInstance() {
    static EventManager instance;
    return instance;
}

bool EventManager::RegisterEventToEntityType(EventType eventType, AssetStringID entityTypeString) {
	OVERRIDE_LOG_NAME("Events");
    if (eventsToEntityTypes.find(eventType) != eventsToEntityTypes.end()) {
		std::vector<AssetStringID> entityTypeStrings = eventsToEntityTypes.find(eventType)->second;
		entityTypeStrings.push_back(entityTypeString);
		for (int iterator = 0; iterator < entityTypeStrings.size(); iterator++) {
			if (entityTypeStrings[iterator].CanonicalName() == entityTypeString.CanonicalName()) {
				ERR("Tried to register the same event to the same entity type twice");
				return false;
			}
		}
        eventsToEntityTypes[eventType] = entityTypeStrings;
        return true;
    }

	std::vector<AssetStringID> entityTypeStrings;
	entityTypeStrings.push_back(entityTypeString);
	eventsToEntityTypes[eventType] = entityTypeStrings;
	return true;
}

void EventManager::RegisterFunctionToEvent(EventType eventType, std::function<void(EventData&)> callback) {
    OVERRIDE_LOG_NAME("Events");
    if (eventsToFunctions.find(eventType) == eventsToFunctions.end()) {
		std::vector<std::function<void(EventData&)>> functions;
		functions.push_back(callback);
        eventsToFunctions[eventType] = functions;
        return;
    }

	eventsToFunctions.find(eventType)->second.push_back(callback);
	eventsToFunctions[eventType] = eventsToFunctions.find(eventType)->second;
}

bool EventManager::TriggerEvent(EventType eventType, EventData eventData) {
	OVERRIDE_LOG_NAME("Event Manager");

	eventDatas.resize(eventData.dataSize);
    memcpy(eventDatas.data(), eventData.data, eventData.dataSize);

    EventData copiedData(eventDatas.data(), eventDatas.size());

	ModHandler& modHandler = ModHandler::GetInstance();
	const void* arguments[] = {copiedData.data};

	bool foundEntity = false;
	auto entityTypes = eventsToEntityTypes.find(eventType);
	if (entityTypes != eventsToEntityTypes.end()) {
		foundEntity = true;
		std::vector<std::vector<Entity*>> entities;

		for (int iterator = 0; iterator < entityTypes->second.size(); iterator++) {
			entities.push_back(EntityManager::GetInstance().GetEntitesOfType(entityTypes->second[iterator]));
		}

		for (int iterator = 0; iterator < entities.size(); iterator++) {
			modHandler.CallWasmFunction("GetEvent", arguments);
		}
	}

	bool foundFunction = false;
	auto functions = eventsToFunctions.find(eventType);
	if (functions != eventsToFunctions.end()) {
		foundFunction = true;
		
		for (int iterator = 0; iterator < functions->second.size(); iterator++) {
			functions->second[iterator](copiedData);
		}
	}

	if (!foundEntity && !foundFunction) {
		//WARN("Triggered event with no connected callbacks!");
	}
	return foundEntity || foundFunction;
}

void EventManager::Delete() {
    OVERRIDE_LOG_NAME("Events");
}