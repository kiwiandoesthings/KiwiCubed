#include "Events.h"


EventManager::EventManager() = default;
EventManager::~EventManager() = default;

EventManager& EventManager::GetInstance() {
    static EventManager instance;
    return instance;
}

void EventManager::RegisterEvent(const std::string& eventName) {
    OVERRIDE_LOG_NAME("Events");
    if (eventMap.find(eventName) != eventMap.end()) {
        WARN("Tried to register event \"" + eventName + "\" twice");
        return;
    }

    registeredEvents.emplace_back(eventName);
    Event* event = &registeredEvents.back();
    eventMap.insert({eventName, event});
}

void EventManager::UnregisterEvent(const std::string& eventName) {
    OVERRIDE_LOG_NAME("Events");
    auto it = std::remove_if(registeredEvents.begin(), registeredEvents.end(),
        [&eventName](const Event& event) { return event.eventName == eventName; });
    if (it == registeredEvents.end()) {
        WARN("Tried to unregister non-existent event \"" + eventName + "\"");
    }
    registeredEvents.erase(it, registeredEvents.end());
    eventMap.erase(eventName);
}

void EventManager::AddEventToDo(const std::string& eventName, std::function<void(Event&)> eventTodo) {
    OVERRIDE_LOG_NAME("Events");
    auto event = eventMap.find(eventName);
    if (event != eventMap.end()) {
        event->second->AddToDo(std::move(eventTodo));
    } else {
        WARN("Tried to trigger non-existent event \"" + eventName + "\"");
    }
}

Event::~Event() = default;

void Event::SetData(const std::string& key, std::any data) {
    eventData[key] = std::move(data);
}

void Event::TriggerEvent() {
    for (unsigned int i = 0; i < eventToDo.size(); i++) {
        eventToDo[i](*this);
    }
    eventData.clear();
}

void Event::AddToDo(std::function<void(Event&)> todo) {
    eventToDo.emplace_back(std::move(todo));
}