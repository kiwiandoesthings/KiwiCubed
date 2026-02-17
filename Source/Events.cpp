#include "Events.h"


std::unordered_map<EventType, std::vector<AssetStringID>> EventManager::eventsToEntityTypes;
std::unordered_map<EventType, std::vector<std::function<void(EventData&)>>> EventManager::eventsToFunctions;
std::unordered_map<EventType, std::vector<std::string>> EventManager::eventsToScripts;
std::vector<uint8_t> EventManager::eventDatas;


EventManager& EventManager::GetInstance() {
    static EventManager instance;
    return instance;
}

bool EventManager::RegisterEventToEntityType(EventType eventType, const AssetStringID& entityTypeString) {
	OVERRIDE_LOG_NAME("Event Manager");
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

void EventManager::RegisterFunctionToEvent(EventType eventType, std::function<void(const EventData&)> callback) {
    OVERRIDE_LOG_NAME("Event Manager");
    if (eventsToFunctions.find(eventType) == eventsToFunctions.end()) {
		std::vector<std::function<void(EventData&)>> functions;
		functions.push_back(callback);
        eventsToFunctions[eventType] = functions;
        return;
    }

	eventsToFunctions.find(eventType)->second.push_back(callback);
	eventsToFunctions[eventType] = eventsToFunctions.find(eventType)->second;
}

void EventManager::RegisterScriptToEvent(EventType eventType, const std::string& moduleName) {
	eventsToScripts[eventType].push_back(moduleName);
}

bool EventManager::TriggerEvent(EventType eventType, const EventData eventData) {
	OVERRIDE_LOG_NAME("Event Triggering");

	if (eventData.verification != VerificationNumber) {
		ERR("EventData was corruped somewhere, verification number did not match");
		psnip_trap();
	}

	ModHandler& modHandler = ModHandler::GetInstance();
	const void* arguments[] = {eventData.data};

	bool foundEntity = false;
	auto entityTypes = eventsToEntityTypes.find(eventType);
	if (entityTypes != eventsToEntityTypes.end()) {
		foundEntity = true;
		std::vector<siv::id> entities;

		for (int type = 0; type < entityTypes->second.size(); type++) {
			std::vector<siv::id> entitiesOfType;
			entitiesOfType = EntityManager::GetInstance().GetEntitesOfType(entityTypes->second[type]);
			for (int entity = 0; entity < entitiesOfType.size(); entity++) {
				entities.push_back(entitiesOfType[entity]);
			}
		}

		for (int iterator = 0; iterator < entities.size(); iterator++) {
			std::string functionName = "EntityGet" + eventTypeStrings[eventType];

			if (eventType == EVENT_WORLD_TICK) {
				EventWorldTick* event = static_cast<EventWorldTick*>(eventData.data);
				uint64_t entityId = static_cast<uint64_t>(entities[iterator]); 
    			std::vector<void*> arguments = {event, &entityId};
				std::vector<int> argumentTypes = {asTYPEID_APPOBJECT, asTYPEID_UINT64};
				modHandler.CallModFunction("kiwicubed", functionName, arguments, argumentTypes);
			}
			if (eventType == EVENT_WORLD_PLAYER_BLOCK) {
				EventWorldPlayerBlock* event = static_cast<EventWorldPlayerBlock*>(eventData.data);
				std::vector<void*> arguments = {event, &entities[iterator]};
				std::vector<int> argumentTypes = {asTYPEID_APPOBJECT, asTYPEID_UINT64};
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

	bool foundScript = false;
	auto scripts = eventsToScripts.find(eventType);
	if (scripts != eventsToScripts.end()) {
		foundScript = true;

		for (int iterator = 0; iterator < scripts->second.size(); iterator++) {
			std::string functionName = "Get" + eventTypeStrings[eventType];

			if (eventType == EVENT_WORLD_TICK) {
				EventWorldTick* event = static_cast<EventWorldTick*>(eventData.data);
				std::vector<void*> arguments = {event};
				std::vector<int> argumentTypes = {asTYPEID_APPOBJECT};
				modHandler.CallModFunction(scripts->second[iterator], functionName, arguments, argumentTypes);
			}
			if (eventType == EVENT_WORLD_PLAYER_BLOCK) {
				EventWorldPlayerBlock* event = static_cast<EventWorldPlayerBlock*>(eventData.data);
				std::vector<void*> arguments = {event};
				std::vector<int> argumentTypes = {asTYPEID_APPOBJECT};
				modHandler.CallModFunction(scripts->second[iterator], functionName, arguments, argumentTypes);
			}
		}
	}

	if (!foundEntity && !foundFunction && !foundScript) {
		//WARN("Triggered event with no connected callbacks!");
	}
	return foundEntity || foundFunction;
}

void EventManager::Delete() {
    OVERRIDE_LOG_NAME("Events");
}