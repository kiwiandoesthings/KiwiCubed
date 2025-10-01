#pragma once


class Globals {
    public:
        static Globals& GetInstance(); 
        float deltaTime = 0.0f;
        bool debugMode = false;
    
    private:
        Globals() = default;
        ~Globals() = default;
    
        Globals(const Globals&) = delete;
        Globals& operator=(const Globals&) = delete;
};