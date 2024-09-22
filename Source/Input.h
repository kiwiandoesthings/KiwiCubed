#pragma once

#include <GLFW/glfw3.h>

#include <functional>
#include <iostream>
#include <unordered_map>


class InputHandler {
    public:
        using KeyCallback = std::function<void()>;
        using MouseButtonCallback = std::function<void()>;
        using MousePositionCallback = std::function<void>();
    
        InputHandler() : window(window) {}
    
        virtual void Setup(GLFWwindow* window) {
            InputHandler::window = window;
    
            // Set the callback functions
            glfwSetKeyCallback(window, KeyCallbackHandler);
            glfwSetMouseButtonCallback(window, MouseButtonCallbackHandler);
            glfwSetCursorPosCallback(window, MousePositionCallbackHandler);
        }
    
        void RegisterKeyCallback(int key, KeyCallback callback) {
            keyCallbacks[key] = callback;
        }
    
        void RegisterMouseButtonCallback(int button, MouseButtonCallback callback) {
            mouseButtonCallbacks[button] = callback;
        }
    
        void GetMousePosition(double& xpos, double& ypos) {
            xpos = mouseX;
            ypos = mouseY;
        }
    private:
        GLFWwindow* window;
        static std::unordered_map<int, bool> keyStates;
        static std::unordered_map<int, bool> mouseButtonStates;
        static double mouseX, mouseY;
        static std::unordered_map<int, KeyCallback> keyCallbacks;
        static std::unordered_map<int, MouseButtonCallback> mouseButtonCallbacks;

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

       static void MouseButtonCallbackHandler(GLFWwindow* window, int button, int action, int mods) {
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

       static void MousePositionCallbackHandler(GLFWwindow* window, double xpos, double ypos) {
           mouseX = xpos;
           mouseY = ypos;
       }
};









//class InputHandler {
//    public:
//        InputHandler() {
//            instance = this;
//        }
//
//        virtual void Setup(GLFWwindow* window) {
//            InputHandler::window = window;
//
//            // Set the callback functions
//            glfwSetKeyCallback(window, KeyCallbackHandlerImplementation);
//            glfwSetMouseButtonCallback(window, MouseButtonCallbackHandlerImplementation);
//            glfwSetCursorPosCallback(window, MousePositionCallbackHandlerImplementation);
//        }
//
//        virtual  void RegisterKeyCallback(int key, KeyCallback callback) {
//            keyCallbacks[key] = callback;
//        }
//
//        virtual void RegisterMouseButtonCallback(int button, MouseButtonCallback callback) {
//            mouseButtonCallbacks[button] = callback;
//        }
//
//        virtual void GetMousePosition(double& xpos, double& ypos) {
//            xpos = mouseX;
//            ypos = mouseY;
//        }
//
//    private:
//        GLFWwindow* window;
//        static std::unordered_map<int, bool> keyStates;
//        static std::unordered_map<int, bool> mouseButtonStates;
//        static double mouseX, mouseY;
//        static std::unordered_map<int, KeyCallback> keyCallbacks;
//        static std::unordered_map<int, MouseButtonCallback> mouseButtonCallbacks;
//    
//    protected:
//        static void KeyCallbackHandlerImplementation(GLFWwindow* window, int key, int scancode, int action, int mods) override {
//            if (action == GLFW_PRESS) {
//                keyStates[key] = true;
//        
//                // If a callback is registered for this key, invoke it
//                if (keyCallbacks.find(key) != keyCallbacks.end()) {
//                    keyCallbacks[key]();
//                }
//            }
//            else if (action == GLFW_RELEASE) {
//                keyStates[key] = false;
//            }
//        }
//
//       static void MouseButtonCallbackHandlerImplementation(GLFWwindow* window, int button, int action, int mods) override {
//            if (action == GLFW_PRESS) {
//                mouseButtonStates[button] = true;
//
//                // If a callback is registered for this button, invoke it
//                if (mouseButtonCallbacks.find(button) != mouseButtonCallbacks.end()) {
//                    mouseButtonCallbacks[button]();
//                }
//            }
//            else if (action == GLFW_RELEASE) {
//                mouseButtonStates[button] = false;
//            }
//        }
//
//       static void MousePositionCallbackHandlerImplementation   (GLFWwindow* window, double xpos, double ypos) override {
//           mouseX = xpos;
//           mouseY = ypos;
//       }
//};