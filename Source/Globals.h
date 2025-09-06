#pragma once


class Globals {
public:
    static Globals& GetInstance(); 
    float deltaTime = 0.0f;

private:
    Globals() = default;
    ~Globals() = default;

    Globals(const Globals&) = delete;
    Globals& operator=(const Globals&) = delete;
};