#pragma once

#include <klogger.hpp>

#include <stdint.h>


extern "C" {
    void KC_Log(const char* message);
    void KC_AddEventToDo(const char* eventName, const char* functionName);
}