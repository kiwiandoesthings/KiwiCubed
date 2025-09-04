#include "Input.h"


std::unordered_map<int, bool> InputHandler::keyStates;
std::unordered_map<int, bool> InputHandler::mouseButtonStates;
std::unordered_map<bool, bool> InputHandler::scrollStates;

std::unordered_map<int, std::vector<KeyCallbackWrapper>> InputHandler::keyCallbacks;
std::unordered_map<int, std::vector<MouseButtonCallbackWrapper>> InputHandler::mouseButtonCallbacks;
std::unordered_map<bool, std::vector<ScrollCallbackWrapper>> InputHandler::scrollCallbacks;

std::vector<InputHandler*> InputHandler::instances;