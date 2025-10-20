#include "Events.h"

EventManager& EventManager::GetInstance() {
    static EventManager instance;
    return instance;
}

void EventManager::RegisterEvent(const std::string& eventName) {
    OVERRIDE_LOG_NAME("Events");
    if (eventMap.find(eventName) != eventMap.end()) {
        WARN("Tried to register event \"" + eventName + "\" twice, aborting");
        return;
    }

    auto event = std::make_unique<Event>(eventName);
    Event* eventPtr = event.get(); // store raw pointer in map
    eventMap[eventName] = eventPtr;
    registeredEvents.push_back(std::move(event));
    INFO("Successfully registered event \"" + eventName + "\"");
}

std::unordered_map<std::string, Event*>::iterator EventManager::DeregisterEvent(const std::string& eventName) {
    OVERRIDE_LOG_NAME("Events");
    if (eventName == "event/blank") {
        return eventMap.end();
    }
    auto iterator = eventMap.find(eventName);
    if (iterator == eventMap.end()) {
        WARN("Tried to deregister non-existent event \"" + eventName + "\", aborting");
        return eventMap.end();
    }

    std::string eventNameCopy = eventName;

    registeredEvents.erase(
        std::remove_if(
            registeredEvents.begin(),
            registeredEvents.end(),
            [&eventName](const std::unique_ptr<Event>& event) { return event->eventName == eventName; }
        ),
        registeredEvents.end()
    );

    INFO("Successfully deregistered event \"" + eventNameCopy + "\"");
    return eventMap.erase(iterator);
}

void EventManager::AddEventToDo(const std::string& eventName, std::function<void(Event&)> eventTodo) {
    OVERRIDE_LOG_NAME("Events");
    auto it = eventMap.find(eventName);
    if (it != eventMap.end()) {
        it->second->AddToDo(std::move(eventTodo));
    } else {
        WARN("Tried to add a todo to non-existent event \"" + eventName + "\"");
    }
}

void EventManager::Delete() {
    OVERRIDE_LOG_NAME("Events");

    std::unordered_map<std::string, Event*> eventMapCopy = eventMap;

    for (auto iterator = eventMapCopy.begin(); iterator != eventMapCopy.end(); iterator++) {
        DeregisterEvent(iterator->second->eventName);
    }
}

void Event::SetData(const std::string& key, std::any data) {
    eventData[key] = std::move(data);
}

void Event::TriggerEvent() {
    for (auto& todo : eventToDo) {
        todo(*this);
    }
    eventData.clear();
}

void Event::AddToDo(std::function<void(Event&)> todo) {
    eventToDo.emplace_back(std::move(todo));
}