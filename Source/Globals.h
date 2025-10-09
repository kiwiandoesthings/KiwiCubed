#pragma once


class Globals {
    public:
        static Globals& GetInstance(); 
        float deltaTime = 0.0f;
        //#ifdef KIWI_DEBUG_MODE
        //    bool debugMode = true;
        //#else
        //    bool debugMode = false;
        //#endif
        bool debugMode = false;
    
    private:
        Globals() = default;
        ~Globals() = default;
    
        Globals(const Globals&) = delete;
        Globals& operator=(const Globals&) = delete;
};