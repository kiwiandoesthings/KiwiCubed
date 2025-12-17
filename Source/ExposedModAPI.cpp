#include "ExposedModAPI.h"
#include <iostream>
#include "AssetManager.h"
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

    unsigned int KC_GetTextureNumericalID(const char* modName, const char* assetName) {
        unsigned int id = assetManager.GetNumericalID(AssetStringID{std::string(modName), std::string(assetName)});
        return id;
    }
}