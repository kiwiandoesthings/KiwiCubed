#include "Input.h"

std::unordered_map<int, bool> InputHandler::keyStates;
std::unordered_map<int, bool> InputHandler::mouseButtonStates;
std::unordered_map<int, std::function<void()>> InputHandler::keyCallbacks;
std::unordered_map<int, std::function<void()>> InputHandler::mouseButtonCallbacks;
std::vector<InputHandler*> InputHandler::instances;
std::map<int, bool> keys;