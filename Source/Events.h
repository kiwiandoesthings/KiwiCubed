#pragma once
#include <klogger.hpp>

#include <any>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>


class Event {
    public:
        std::string eventName;
        
        Event(std::string eventName) : eventName(eventName) {}
        ~Event();

        void SetData(const std::string& key, std::any data);
        template<typename T>
        T* GetData(const std::string& key) {
            OVERRIDE_LOG_NAME("Events");
            auto data = eventData.find(key);
            if (data != eventData.end()) {
                return std::any_cast<T>(&data->second);
            } else {
                WARN("Tried to get non-existent event data \"" + key + "\"");
                return nullptr;
            }
        }

        void TriggerEvent();
        void AddToDo(std::function<void(Event&)> todo);

    private:
        std::unordered_map<std::string, std::any> eventData;
        std::vector<std::function<void(Event&)>> eventToDo;
};

class EventManager {
    public:
        static EventManager& GetInstance();

        void RegisterEvent(const std::string& eventName);
        void UnregisterEvent(const std::string& eventName);

        template<typename... KVs>
        void TriggerEvent(const std::string& eventName, KVs&&... kvs) {
            OVERRIDE_LOG_NAME("Events");
            auto event = eventMap.find(eventName);
            if (event != eventMap.end()) {
                Event* eventPtr = event->second;
                (eventPtr->SetData(std::forward<KVs>(kvs).first, std::forward<KVs>(kvs).second), ...);
                eventPtr->TriggerEvent();
            } else {
                WARN("Tried to trigger non-existent event \"" + eventName + "\"");
            }
        }
        void AddEventToDo(const std::string& eventName, std::function<void(Event&)> eventTodo);

    private:
        EventManager();
        ~EventManager();

        EventManager(const EventManager&) = delete;
        EventManager& operator=(const EventManager&) = delete;

        std::vector<Event> registeredEvents;
        std::unordered_map<std::string, Event*> eventMap;
};