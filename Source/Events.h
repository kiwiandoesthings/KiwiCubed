#pragma once

#include <klogger.hpp>

#include <algorithm>
#include <any>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "EventDefinitions.h"


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
	//  Generic events
    EVENT_WORLD_GENERIC_BLOCK_EVENT,
};


class Event {
    public:
        std::string eventName;
    
        Event(const std::string& eventName) : eventName(eventName) {}
        ~Event() = default;
    
        void SetData(void* newEventData, size_t newEventDataSize);
    
        void TriggerEvent();
        void AddToDo(std::function<void(Event&)> todo);
    
    private:
        void* eventData = nullptr;
		size_t eventDataSize = 0;
        std::vector<std::function<void(Event&)>> eventToDo;
};


class EventManager {
    public:
        static EventManager& GetInstance();
        void Delete();

		//bool RegisterEventToEntityType(EventType eventType)
        void RegisterEvent(const std::string& eventName);
        std::unordered_map<std::string, Event*>::iterator DeregisterEvent(const std::string& eventName);

        template<typename... KVs>
        void TriggerEvent(const std::string& eventName, KVs&&... kvs) {
            OVERRIDE_LOG_NAME("Events");
            auto it = eventMap.find(eventName);
            if (it != eventMap.end()) {
                Event* eventPtr = it->second;
                (eventPtr->SetData(std::forward<KVs>(kvs).first, std::forward<KVs>(kvs).second), ...);
                eventPtr->TriggerEvent();
            } else {
                WARN("Tried to trigger non-existent event \"" + eventName + "\"");
            }
        }

        void AddEventToDo(const std::string& eventName, std::function<void(Event&)> eventTodo);

    private:
        EventManager() = default;
        ~EventManager() = default;

        EventManager(const EventManager&) = delete;
        EventManager& operator=(const EventManager&) = delete;

        std::vector<std::unique_ptr<Event>> registeredEvents;
        std::unordered_map<std::string, Event*> eventMap;
};