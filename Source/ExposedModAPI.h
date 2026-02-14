#pragma once

#include <klogger.hpp>

#include <stdint.h>

enum EventType;


extern "C" {
    void KC_Log(const char* message);
	void KC_RegisterEventToEntityType(int eventType, const char* modName, const char* assetName);
    void KC_RegisterFunctionToEvent(int eventType, const char* functionName);
    unsigned int KC_GetTextureNumericalID(const char* modName, const char* assetName);
}