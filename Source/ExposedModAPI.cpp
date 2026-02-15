#include "ExposedModAPI.h"
#include <iostream>
#include "AssetManager.h"
#include "Events.h"
#include "ModHandler.h"
#include "AssetDefinitions.h"

extern "C" {
    void KC_Log(const char* message) {
        OVERRIDE_LOG_NAME("Mod Script");
        EXT(std::string(message));
    }

	void KC_RegisterEventToEntityType(int eventType, const char* modName, const char* assetName) {
		EventManager::GetInstance().RegisterEventToEntityType(static_cast<EventType>(eventType), assetManager.StringToAssetStruct(std::string(modName) + std::string(assetName), "entity"));
	}

    void KC_RegisterFunctionToEvent(int eventType, const char* functionName) {
        std::string stringFunctionName = std::string(functionName);

        EventManager::GetInstance().RegisterFunctionToEvent(static_cast<EventType>(eventType), [stringFunctionName](EventData& eventData) {
            ModHandler& modHandler = ModHandler::GetInstance();
			const void* arguments[] = {eventData.data};
            //modHandler.CallWasmFunction(stringFunctionName, arguments);
        });
    }

    unsigned int KC_GetTextureNumericalID(const char* modName, const char* assetName) {
        unsigned int id = assetManager.GetNumericalID(AssetStringID{std::string(modName), std::string(assetName)});
        return id;
    }
}