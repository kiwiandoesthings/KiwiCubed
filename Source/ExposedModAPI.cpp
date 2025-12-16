#include "ExposedModAPI.h"
#include <iostream>
#include "Events.h"
#include "ModHandler.h"

extern "C" {
    void KC_Log(const char* message) {
        OVERRIDE_LOG_NAME("Mod Script");
        EXT(std::string(message));
    }

    void KC_AddEventToDo(const char* eventName, const char* functionName) {
        std::string stringFunctionName = std::string(functionName);

        EventManager::GetInstance().AddEventToDo(eventName, [stringFunctionName](Event& event) {
            ModHandler& modHandler = ModHandler::GetInstance();
            modHandler.SetCurrentlyProcessingEvent(&event);
            modHandler.CallWasmFunction(stringFunctionName);
            modHandler.SetCurrentlyProcessingEvent(nullptr);
        });
    }
}