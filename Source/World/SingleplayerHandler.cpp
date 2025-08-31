#include "SingleplayerHandler.h"


void SingleplayerHandler::StartSingleplayerWorld(DebugRenderer& debugRenderer) {
	singleplayerWorld = std::make_unique<World>(5, this);
	isLoadedIntoSingleplayerWorld = true;
	singleplayerWorld->Setup();
	debugRenderer.SetupBuffers(singleplayerWorld->GetChunkDebugVisualizationVertices(), singleplayerWorld->GetChunkDebugVisualizationIndices(), singleplayerWorld->GetChunkOrigins());

	EventManager& eventManager = EventManager::GetInstance();
	eventManager.RegisterEvent("EntityMovedChunk");
	eventManager.AddEventToDo("EntityMovedChunk", [&](Event& event) {
		singleplayerWorld->GenerateChunksAroundPosition(event);
	});
	singleplayerWorld->StartTickThread();
}

void SingleplayerHandler::EndSingleplayerWorld() {
	OVERRIDE_LOG_NAME("Singleplayer Handler");
	isLoadedIntoSingleplayerWorld = false;
	singleplayerWorld->Delete();
	INFO("Exiting singleplayer world");
}

void SingleplayerHandler::Delete() {
	EndSingleplayerWorld();
}