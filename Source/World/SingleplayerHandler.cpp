#include "SingleplayerHandler.h"


void SingleplayerHandler::Setup() {
	singleplayerWorld.Setup(window);

	EventManager& eventManager = EventManager::GetInstance();
	eventManager.RegisterEvent("EntityMovedChunk");
	eventManager.AddEventToDo("EntityMovedChunk", [&](Event& event) {
		singleplayerWorld.GenerateChunksAroundPosition(event);
	});
}

void SingleplayerHandler::StartSingleplayerWorld() {
	singleplayerWorld.StartTickThread();
}

void SingleplayerHandler::EndSingleplayerWorld() {
	OVERRIDE_LOG_NAME("Singleplayer Handler");
	isLoadedIntoSingleplayerWorld = false;
	singleplayerWorld.Delete();
	INFO("Exiting singleplayer world");
}

void SingleplayerHandler::Delete() {
	EndSingleplayerWorld();
}