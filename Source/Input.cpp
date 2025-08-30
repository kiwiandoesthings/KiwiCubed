#include "Input.h"


std::unordered_map<int, bool> InputHandler::keyStates;
std::unordered_map<int, bool> InputHandler::mouseButtonStates;
std::unordered_map<bool, bool> InputHandler::scrollStates;

std::unordered_map<int, std::vector<InputHandler::KeyCallback>> InputHandler::keyCallbacks;
std::unordered_map<int, std::vector<InputHandler::MouseButtonCallback>> InputHandler::mouseButtonCallbacks;
std::unordered_map<bool, std::vector<InputHandler::ScrollCallback>> InputHandler::scrollCallbacks;

std::vector<InputHandler*> InputHandler::instances;