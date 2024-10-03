#pragma once

#include <GLFW/glfw3.h>

#include <functional>
#include <iostream>
#include <map>
#include <unordered_map>

class Window;

class InputHandler {
    public:
        using KeyCallback = std::function<void()>;
        using MouseButtonCallback = std::function<void()>;
    
        InputHandler() : window(window) {}
    
        void SetupCallbacks(GLFWwindow* window) {
            InputHandler::window = window;

            glfwSetKeyCallback(window, KeyCallbackHandler);
            glfwSetMouseButtonCallback(window, MouseButtonCallbackHandler);
        }

        void SetupKeyStates(GLFWwindow* window, std::vector<int> monitoredKeys) {
            for (int key : monitoredKeys) {
                keys[key] = false;
            }

            instances.push_back(this);
        }
    
        void RegisterKeyCallback(std::vector<int> keys, KeyCallback callback) {
            std::cout << "[InputHandler / Info] Key callbacks registered for keys: ( ";
            for (int key : keys) {
                keyCallbacks[key] = callback;
                const char* keyName = glfwGetKeyName(key, 0);
                if (keyName == NULL) {
                    std::cout << "keycode: " << key << " ";
                    continue;
                };
                std::cout << "\'" << keyName << "\' ";
            }
            auto iterator = std::find(instances.begin(), instances.end(), this);
            int index = static_cast<int>(iterator - instances.begin());
            std::cout << ") for input instance[" << index << "]" << std::endl;
        }
    
        void RegisterMouseButtonCallback(int button, MouseButtonCallback callback) {
            mouseButtonCallbacks[button] = callback;
        }

        bool GetKeyState(int key) const {
            auto iterator = keyStates.find(key);
            return iterator != keyStates.end() && iterator->second;
        }
    
    private:
        static std::unordered_map<int, bool> keyStates;
        static std::unordered_map<int, bool> mouseButtonStates;
        static std::unordered_map<int, KeyCallback> keyCallbacks;
        static std::unordered_map<int, MouseButtonCallback> mouseButtonCallbacks;
        static std::vector<InputHandler*> instances;
        std::map<int, bool> keys;
        GLFWwindow* window;
    
        // Key callback handler
        static void KeyCallbackHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS) {
                keyStates[key] = true;
    
                // If a callback is registered for this key, invoke it
                if (keyCallbacks.find(key) != keyCallbacks.end()) {
                    keyCallbacks[key]();
                }
            }
            else if (action == GLFW_RELEASE) {
                keyStates[key] = false;
            }
        }
    
        // Mouse button callback handler
        static  void MouseButtonCallbackHandler(GLFWwindow* window, int button, int action, int mods) {
            if (action == GLFW_PRESS) {
                mouseButtonStates[button] = true;
    
                // If a callback is registered for this button, invoke it
                if (mouseButtonCallbacks.find(button) != mouseButtonCallbacks.end()) {
                    mouseButtonCallbacks[button]();
                }
            }
            else if (action == GLFW_RELEASE) {
                mouseButtonStates[button] = false;
            }
        }
};