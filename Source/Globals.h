#pragma once

#include "string"


class Globals {
    public:
        static Globals& GetInstance(); 
        float deltaTime = 0.0f;
        unsigned long long frameCount = 0;
        std::string gameVersion = "";
        #ifdef KIWI_DEBUG_MODE
            bool debugMode = true;
        #else
            bool debugMode = false;
        #endif
    
    private:
        Globals() = default;
        ~Globals() = default;
    
        Globals(const Globals&) = delete;
        Globals& operator=(const Globals&) = delete;
};