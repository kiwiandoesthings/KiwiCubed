#include "Events.h"


std::unordered_map<EventType, std::vector<AssetStringID>> EventManager::eventsToEntityTypes;
std::unordered_map<EventType, std::vector<std::function<void(EventData&)>>> EventManager::eventsToFunctions;
std::vector<uint8_t> EventManager::eventDatas;


EventManager& EventManager::GetInstance() {
    static EventManager instance;
    return instance;
}

bool EventManager::RegisterEventToEntityType(EventType eventType, const AssetStringID& entityTypeString) {
	OVERRIDE_LOG_NAME("Events");
	std::vector<AssetStringID>& entityTypeStrings = eventsToEntityTypes[eventType];
	for (int iterator = 0; iterator < entityTypeStrings.size(); iterator++) {
		if (entityTypeStrings[iterator].CanonicalName() == entityTypeString.CanonicalName()) {
			ERR("Tried to register the same event to the same entity type twice");
			return false;
		}
	}
	entityTypeStrings.push_back(entityTypeString);

	INFO("Successfully registered event with numerical ID {" + std::to_string(static_cast<int>(eventType)) + "} to entity type with string ID \"" + entityTypeString.CanonicalName() + "\"");
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

bool EventManager::TriggerEvent(EventType eventType, const EventData eventData) {
	OVERRIDE_LOG_NAME("Event Manager");

	ModHandler& modHandler = ModHandler::GetInstance();
	const void* arguments[] = {eventData.data};

	bool foundEntity = false;
	auto entityTypes = eventsToEntityTypes.find(eventType);
	if (entityTypes != eventsToEntityTypes.end()) {
		foundEntity = true;
		std::vector<std::vector<Entity*>> entities;

		for (int iterator = 0; iterator < entityTypes->second.size(); iterator++) {
			entities.push_back(EntityManager::GetInstance().GetEntitesOfType(entityTypes->second[iterator]));
		}

		for (int iterator = 0; iterator < entities.size(); iterator++) {
			std::string functionName = "Get" + eventTypeStrings[eventType];

			if (eventType == EVENT_WORLD_PLAYER_BLOCK) {
				WorldPlayerBlockEvent* event = static_cast<WorldPlayerBlockEvent*>(eventData.data);
				std::vector<void*> arguments = {event};
				std::vector<int> argumentTypes = {asTYPEID_APPOBJECT};
				modHandler.CallModFunction("kiwicubed", functionName, arguments, argumentTypes);
			}
		}
	}

	bool foundFunction = false;
	auto functions = eventsToFunctions.find(eventType);
	if (functions != eventsToFunctions.end()) {
		foundFunction = true;
		
		for (int iterator = 0; iterator < functions->second.size(); iterator++) {
			functions->second[iterator](const_cast<EventData&>(eventData));
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