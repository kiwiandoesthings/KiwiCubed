#include "SingleplayerHandler.h"


void SingleplayerHandler::Setup() {
	singleplayerWorld.Setup(window);
}

void SingleplayerHandler::StartSingleplayerWorld() {
	singleplayerWorld.StartTickThread();
}

void SingleplayerHandler::EndSingleplayerWorld() {
	isLoadedIntoSingleplayerWorld = false;
	singleplayerWorld.StopTickThread();
	INFO("Exiting singleplayer world");
}

void SingleplayerHandler::Delete() {
	EndSingleplayerWorld();
}