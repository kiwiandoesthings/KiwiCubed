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

struct KeyCallbackWrapper {
    std::function<void(int key)> callback;
    unsigned int id;
    std::string instanceID;
};

struct MouseButtonCallbackWrapper {
    std::function<void(int button)> callback;
    unsigned int id;
    std::string instanceID;
};

struct ScrollCallbackWrapper {
    std::function<void(double offset)> callback;
    unsigned int id;
    std::string instanceID;
};

class Window;

class InputHandler {
public:
    using KeyCallback = std::function<void(int key)>;
    using MouseButtonCallback = std::function<void(int button)>;
    using ScrollCallback = std::function<void(double offset)>;

    InputHandler(std::string id) : instanceID(id) {}

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

    unsigned int RegisterKeyCallback(int key, KeyCallback callback) {
        OVERRIDE_LOG_NAME("Input Handler");
        keyCallbacks[key].push_back(KeyCallbackWrapper{callback, latestID + 1, instanceID});

        const char* keyName = glfwGetKeyName(key, 0);
        std::stringstream outputString;
        outputString << "Key callback registered for key: (";
        if (keyName == nullptr) outputString << "keycode: " << key;
        else outputString << "'" << keyName << "'";
        int index = static_cast<int>(std::find(instances.begin(), instances.end(), this) - instances.begin());
        outputString << ") for input instance[" << index << "] with callback ID of " << latestID + 1 << " and instance ID of " << instanceID;
        INFO(outputString.str());

        latestID += 1;

        return latestID;
    }

    unsigned int RegisterMouseButtonCallback(int button, MouseButtonCallback callback) {
        OVERRIDE_LOG_NAME("Input Handler");
        mouseButtonCallbacks[button].push_back(MouseButtonCallbackWrapper{callback, latestID + 1, instanceID});

        std::stringstream outputString;
        outputString << "Mouse button callback registered for mouse button: (" << button << ")";
        int index = static_cast<int>(std::find(instances.begin(), instances.end(), this) - instances.begin());
        outputString << " for input instance[" << index << "] with callback ID of " << latestID + 1 << " and instance ID of " << instanceID;
        INFO(outputString.str());

        latestID += 1;

        return latestID;
    }

    unsigned int RegisterScrollCallback(bool direction, ScrollCallback callback) {
        OVERRIDE_LOG_NAME("Input Handler");
        scrollCallbacks[direction].push_back(ScrollCallbackWrapper{callback, latestID + 1, instanceID});

        std::stringstream outputString;
        outputString << "Scroll callback registered for scroll direction: (" << (direction ? "y-axis" : "x-axis") << ")";
        int index = static_cast<int>(std::find(instances.begin(), instances.end(), this) - instances.begin());
        outputString << " for input instance[" << index << "] with callback ID of " << latestID + 1 << " and instance ID of " << instanceID;
        INFO(outputString.str());

        latestID += 1;

        return latestID;
    }

    void DeregisterCallback(unsigned int id, std::string instanceID) {
        for (auto& [key, vector] : keyCallbacks) {
            for (int iterator = 0; iterator < vector.size(); ++ iterator) {
                if (vector[iterator].id == id && vector[iterator].instanceID == instanceID) {
                    DeregisterKeyCallback(key, id, instanceID);
                    return;
                }
            }
        }

        for (auto& [key, vector] : mouseButtonCallbacks) {
            for (int iterator = 0; iterator < vector.size(); ++ iterator) {
                if (vector[iterator].id == id && vector[iterator].instanceID == instanceID) {
                    DeregisterMouseButtonCallback(key, id, instanceID);
                    return;
                }
            }
        }

        for (auto& [key, vector] : scrollCallbacks) {
            for (int iterator = 0; iterator < vector.size(); ++ iterator) {
                if (vector[iterator].id == id && vector[iterator].instanceID == instanceID) {
                    DeregisterScrollCallback(key, id, instanceID);
                    return;
                }
            }
        }
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

    static std::unordered_map<int, std::vector<KeyCallbackWrapper>> keyCallbacks;
    static std::unordered_map<int, std::vector<MouseButtonCallbackWrapper>> mouseButtonCallbacks;
    static std::unordered_map<bool, std::vector<ScrollCallbackWrapper>> scrollCallbacks;

    std::string instanceID;
    unsigned int latestID = 0;
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
                callback.callback(key);   
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
                    callback.callback(button);
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
                callback.callback(xoffset);
            }
        }

        auto iteratorY = scrollCallbacks.find(true);
        if (iteratorY != scrollCallbacks.end()) {
            for (auto& callback : iteratorY->second) {
                callback.callback(yoffset);
            }
        }
    }

    void DeregisterKeyCallback(int key, unsigned int id, std::string instanceID) {
        OVERRIDE_LOG_NAME("Input Handler");

        for (int iterator = 0; iterator < keyCallbacks[key].size(); ++iterator) {
            if (keyCallbacks[key][iterator].id == id && keyCallbacks[key][iterator].instanceID == instanceID) {
                keyCallbacks[key].erase(keyCallbacks[key].begin() + iterator);
                break;
            }
        }

        const char* keyName = glfwGetKeyName(key, 0);
        std::stringstream outputString;
        outputString << "Key callback deregistered for key: (";
        if (keyName == nullptr) outputString << "keycode: " << key;
        else outputString << "'" << keyName << "'";
        int index = static_cast<int>(std::find(instances.begin(), instances.end(), this) - instances.begin());
        outputString << ") for input instance[" << index << "] with callback ID of " << id << " and instance ID of " << instanceID;
        INFO(outputString.str());
    }

    void DeregisterMouseButtonCallback(int button, unsigned int id, std::string instanceID) {
        OVERRIDE_LOG_NAME("Input Handler");
        
        for (int iterator = 0; iterator < mouseButtonCallbacks[button].size(); ++iterator) {
            if (mouseButtonCallbacks[button][iterator].id == id && mouseButtonCallbacks[button][iterator].instanceID == instanceID) {
                mouseButtonCallbacks[button].erase(mouseButtonCallbacks[button].begin() + iterator);
                break;
            }
        }

        std::stringstream outputString;
        outputString << "Mouse button callback deregistered for mouse button: (" << button << ")";
        int index = static_cast<int>(std::find(instances.begin(), instances.end(), this) - instances.begin());
        outputString << " for input instance[" << index << "] with callback ID of " << id << " and instance ID of " << instanceID;
        INFO(outputString.str());
    }

    void DeregisterScrollCallback(bool direction, unsigned int id, std::string instanceID) {
        OVERRIDE_LOG_NAME("Input Handler");
        
        for (int iterator = 0; iterator < scrollCallbacks[direction].size(); ++iterator) {
            if (scrollCallbacks[direction][iterator].id == id && scrollCallbacks[direction][iterator].instanceID == instanceID) {
                scrollCallbacks[direction].erase(scrollCallbacks[direction].begin() + iterator);
                break;
            }
        }

        std::stringstream outputString;
        outputString << "Scroll callback deregistered for scroll direction: (" << (direction ? "y-axis" : "x-axis") << ")";
        int index = static_cast<int>(std::find(instances.begin(), instances.end(), this) - instances.begin());
        outputString << " for input instance[" << index << "] with callback ID of " << id << " and instance ID of " << instanceID;
        INFO(outputString.str());
    }
};