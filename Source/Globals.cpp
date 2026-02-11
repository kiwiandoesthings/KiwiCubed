#include "Globals.h"


Globals& Globals::GetInstance() {
    static Globals instance;
    return instance;
}

Globals::Globals() {
    OVERRIDE_LOG_NAME("Globals Initialization");
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
	if (validateConfig) {
		if (windowWidth <= 0) {
    	    ERR("Option \"window_width\" from init_settings.json is invalid (<= 0), setting to default value (640)");
    	    windowWidth = 640;
    	    configJSON["init_settings"]["window_width"] = windowWidth;
    	}
    	if (windowHeight <= 0) {
    	    ERR("Option \"window_height\" from init_settings.json is invalid (<= 0), setting to default value (480)");
    	    windowHeight = 480;
    	    configJSON["init_settings"]["window_height"] = windowHeight;
    	}
		if (!(windowType == "fullscreen" || windowType == "window_borderless" || windowType == "window_maximized" || windowType == "window")) {
    	    ERR("Option \"window_type\" from init_settings.json is invalid (not equal to any of \"fullscreen\", \"window_borderless\", \"window_maximized\", or \"window\"), setting to default value (window_maximized)");
    	    windowType = "window_maximized";
    	    configJSON["init_settings"]["window_type"] = windowType;
    	}
		if (fov <= 0) {
    	    ERR("Option \"fov\" from init_settings.json is invalid (<= 0), setting to default value (80.0f)");
    	    fov = 80.0f;
    	    configJSON["init_settings"]["fov"] = fov;
    	}
	}

    if (overrideDebug) {
        debugMode = debugModeConfig;
    }

	configFile.close();

    std::ofstream configWrite("init_config.json");
	if (!configWrite.is_open()) {
		CRITICAL("Could not open the JSON config file, aborting");
		psnip_trap();
	}
	configWrite << configJSON.dump(1, '\t');
	configWrite.close();

    INFO("Successfully read the JSON config file and stored globals");
}

OrderedJSON& Globals::GetConfigJSON() {
    return configJSON;
}