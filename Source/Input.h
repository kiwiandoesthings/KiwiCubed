#pragma once

#include "SDL_keyboard.h"
#include <SDL.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <istream>
#include <klogger.hpp>
#include <map>
#include <sstream>
#include <unordered_map>

class Window;

class InputHandler {
  public:
    using KeyCallback = std::function<void()>;
    using MouseButtonCallback = std::function<void()>;
    using ScrollCallback = std::function<void(double offset)>;

    InputHandler() { instances.push_back(this); }

    void handle_single_input(const SDL_Event *e) {
        switch (e->type) {
        case SDL_KEYDOWN:
            keyStates[e->key.keysym.scancode] = true;
            if (keyCallbacks.find(e->key.keysym.scancode) != keyCallbacks.end()) {
                keyCallbacks[e->key.keysym.scancode]();
            }
            break;
        case SDL_KEYUP:
            keyStates[e->key.keysym.scancode] = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (mouseButtonCallbacks.find(e->button.button) != mouseButtonCallbacks.end()) {
                mouseButtonCallbacks[e->button.button]();
            }
            mouseButtonStates[e->button.button] = true;
            break;
        case SDL_MOUSEBUTTONUP:
            mouseButtonStates[e->button.button] = false;
            break;
        case SDL_MOUSEWHEEL:
            scrollStates[true] = e->wheel.y > 0;
            scrollStates[false] = e->wheel.x > 0;
            break;
        }
    }

    void RegisterCallbackOnKeys(std::vector<SDL_Scancode> keys, KeyCallback callback) {
        for (SDL_Scancode key : keys) {
            RegisterKeyCallback(key, callback);
        }
    }

    void RegisterKeyCallback(SDL_Scancode key, KeyCallback callback) {
        OVERRIDE_LOG_NAME("InputHandler");

        keyCallbacks[key] = callback;

        INFO(std::format(
            "Key callback registered for {} for instance[{}]", SDL_GetScancodeName(key),
            static_cast<int>(std::find(instances.begin(), instances.end(), this) - instances.begin())
        ));
    }

    void RegisterMouseButtonCallback(int button, MouseButtonCallback callback) { mouseButtonCallbacks[button] = callback; }

    // Use true for y-scroll and false for x-scroll
    void RegisterScrollCallback(bool direction, ScrollCallback callback) { scrollCallbacks[direction] = callback; }

    bool GetKeyState(SDL_Scancode key) const {
        if (keyStates.find(key) != keyStates.end()) {
            return keyStates[key];
        } else {
            return false;
        }
    }

  private:
    static std::unordered_map<SDL_Scancode, bool> keyStates;
    static std::unordered_map<int, bool> mouseButtonStates;
    static std::unordered_map<bool, bool> scrollStates;
    static std::unordered_map<int, KeyCallback> keyCallbacks;
    static std::unordered_map<int, MouseButtonCallback> mouseButtonCallbacks;
    static std::unordered_map<int, ScrollCallback> scrollCallbacks;
    static std::vector<InputHandler *> instances;
};
