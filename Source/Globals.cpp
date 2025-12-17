#include "Globals.h"


Globals& Globals::GetInstance() {
    static Globals instance;
    return instance;
}

Globals::Globals() {
    std::ifstream configFile("init_config.json");

	if (configFile.is_open()) {
        INFO("Successfully opened the JSON config file");
    } else {
        CRITICAL("Failed to open or find the JSON config file, aborting");
        psnip_trap();
    }

	configFile >> configJSON;

    windowWidth = configJSON["init_settings"]["window_width"];
    windowHeight = configJSON["init_settings"]["window_height"];
    windowTitle = configJSON["init_settings"]["window_title"];
    windowType = configJSON["init_settings"]["window_type"];
    debugModeConfig = configJSON["init_settings"]["debug_mode"];
    overrideDebug = configJSON["init_settings"]["override_debug"];
    fov = configJSON["init_settings"]["fov"];

    if (overrideDebug) {
        debugMode = debugModeConfig;
    }

	configFile.close();
}

OrderedJSON& Globals::GetConfigJSON() {
    return configJSON;
}