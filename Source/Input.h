#pragma once

#include <GLFW/glfw3.h>
#include <klogger.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <unordered_map>
#include <sstream>
#include <vector>

#include <glm/vec2.hpp>

class Window;

class InputHandler {
public:
    using KeyCallback = std::function<void(int key)>;
    using MouseButtonCallback = std::function<void(int button)>;
    using ScrollCallback = std::function<void(double offset)>;

    InputHandler() {}

    void SetupCallbacks(GLFWwindow* window) {
        this->window = window;

        glfwSetKeyCallback(window, KeyCallbackHandler);
        glfwSetMouseButtonCallback(window, MouseButtonCallbackHandler);
        glfwSetScrollCallback(window, ScrollCallbackHandler);

        instances.push_back(this);
    }

    void RegisterCallbackOnKeys(std::vector<int> keys, KeyCallback callback) {
        for (int key : keys) {
            RegisterKeyCallback(key, callback);
        }
    }

    void RegisterKeyCallback(int key, KeyCallback callback) {
        OVERRIDE_LOG_NAME("Input Handler");
        keyCallbacks[key].push_back(callback);

        const char* keyName = glfwGetKeyName(key, 0);
        std::stringstream outputString;
        outputString << "Key callback registered for key: (";
        if (keyName == nullptr) outputString << "keycode: " << key;
        else outputString << "'" << keyName << "'";
        int index = static_cast<int>(std::find(instances.begin(), instances.end(), this) - instances.begin());
        outputString << ") for input instance[" << index << "]";
        INFO(outputString.str());
    }

    void RegisterMouseButtonCallback(int button, MouseButtonCallback callback) {
        OVERRIDE_LOG_NAME("Input Handler");
        mouseButtonCallbacks[button].push_back(callback);

        std::stringstream outputString;
        outputString << "Mouse button callback registered for mouse button: (" << button << ")";
        int index = static_cast<int>(std::find(instances.begin(), instances.end(), this) - instances.begin());
        outputString << " for input instance[" << index << "]";
        INFO(outputString.str());
    }

    void RegisterScrollCallback(bool direction, ScrollCallback callback) {
        OVERRIDE_LOG_NAME("Input Handler");
        scrollCallbacks[direction].push_back(callback);

        std::stringstream outputString;
        outputString << "Scroll callback registered for scroll direction: (" << (direction ? "y-axis" : "x-axis") << ")";
        int index = static_cast<int>(std::find(instances.begin(), instances.end(), this) - instances.begin());
        outputString << " for input instance[" << index << "]";
        INFO(outputString.str());
    }

    bool GetKeyState(int key) const {
        auto iterator = keyStates.find(key);
        return iterator != keyStates.end() && iterator->second;
    }

    glm::vec2 GetMousePosition() const {
        double mouseX; 
        double mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        return glm::vec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
    }

    bool GetMouseButtonState(int button) const {
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

private:
    static std::unordered_map<int, bool> keyStates;
    static std::unordered_map<int, bool> mouseButtonStates;
    static std::unordered_map<bool, bool> scrollStates;

    static std::unordered_map<int, std::vector<KeyCallback>> keyCallbacks;
    static std::unordered_map<int, std::vector<MouseButtonCallback>> mouseButtonCallbacks;
    static std::unordered_map<bool, std::vector<ScrollCallback>> scrollCallbacks;

    static std::vector<InputHandler*> instances;
    GLFWwindow* window;

    static void KeyCallbackHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
        if (action == GLFW_PRESS) {
            keyStates[key] = true;
        } else if (action == GLFW_RELEASE) {
            keyStates[key] = false;
            return;
        } else if (action == GLFW_REPEAT) {
            return;
        }

        auto iterator = keyCallbacks.find(key);
        if (iterator != keyCallbacks.end()) {
            for (auto& callback : iterator->second) {
                callback(key);   
            }
        }
    }

    static void MouseButtonCallbackHandler(GLFWwindow* window, int button, int action, int mods) {
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
        if (action == GLFW_PRESS) {
            mouseButtonStates[button] = true;
            auto iterator = mouseButtonCallbacks.find(button);
            if (iterator != mouseButtonCallbacks.end()) {
                for (auto& callback : iterator->second) {
                    callback(button);
                }
            }
        } else if (action == GLFW_RELEASE) {
            mouseButtonStates[button] = false;
        }
    }

    static void ScrollCallbackHandler(GLFWwindow* window, double xoffset, double yoffset) {
        ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
        scrollStates[false] = xoffset > 0;
        scrollStates[true] = yoffset > 0;

        auto iteratorX = scrollCallbacks.find(false);
        if (iteratorX != scrollCallbacks.end()) {
            for (auto& callback : iteratorX->second) {
                callback(xoffset);
            }
        }

        auto iteratorY = scrollCallbacks.find(true);
        if (iteratorY != scrollCallbacks.end()) {
            for (auto& callback : iteratorY->second) {
                callback(yoffset);
            }
        }
    }
};