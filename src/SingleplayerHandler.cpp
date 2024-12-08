#include "SingleplayerHandler.h"


SingleplayerHandler::SingleplayerHandler(Window &newWindow) : window(newWindow), isLoadedIntoSingleplayerWorld(false) {
}

void SingleplayerHandler::Setup() {
	singleplayerWorld.Setup(window);
}

void SingleplayerHandler::StartSingleplayerWorld() {
	singleplayerWorld.StartTickThread();
}

void SingleplayerHandler::EndSingleplayerWorld() {
	isLoadedIntoSingleplayerWorld = false;

	singleplayerWorld.StopTickThread();

	std::cout << "[Singleplayer World Handler / Info] Exiting singleplayer world" << std::endl;
}

void SingleplayerHandler::Delete() {
	EndSingleplayerWorld();
}