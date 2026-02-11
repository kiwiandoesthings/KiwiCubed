#pragma once

#include <klogger.hpp>
#include <debug-trap.h>

#include <fstream>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>


using OrderedJSON = nlohmann::ordered_json;


class Globals {
    public:
        static Globals& GetInstance(); 

        // Code globals
        float deltaTime = 0.0f;
        unsigned long long frameCount = 0;
        unsigned char bitness = 0;
        #ifdef KIWI_DEBUG_MODE
        bool debugMode = true;
        #else
        bool debugMode = false;
        #endif
		bool validateConfig = true;
        
        // Config file globals (all at default values)
        const std::string projectVersion = "0.0.4pre-alpha";
        int windowWidth = 1;
        int windowHeight = 1;
        std::string windowTitle = "";
        std::string windowType = "";
        bool debugModeConfig = false;
        bool overrideDebug = false;
        float fov = 0.0;
        
        OrderedJSON& GetConfigJSON();
        
    private:
        Globals();
        ~Globals() = default;
        
        Globals(const Globals&) = delete;
        Globals& operator=(const Globals&) = delete;

        OrderedJSON configJSON;
};