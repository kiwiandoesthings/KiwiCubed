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

void EventManager::DeregisterEvent(const std::string& eventName) {
    OVERRIDE_LOG_NAME("Events");
    auto it = eventMap.find(eventName);
    if (it == eventMap.end()) {
        WARN("Tried to deregister non-existent event \"" + eventName + "\", aborting");
        return;
    }

    // Remove from vector
    registeredEvents.erase(
        std::remove_if(
            registeredEvents.begin(),
            registeredEvents.end(),
            [&eventName](const std::unique_ptr<Event>& e) { return e->eventName == eventName; }
        ),
        registeredEvents.end()
    );

    // Remove from map
    eventMap.erase(it);
    INFO("Successfully deregistered event \"" + eventName + "\"");
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