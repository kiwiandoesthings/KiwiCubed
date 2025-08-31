#include "SingleplayerHandler.h"


void SingleplayerHandler::StartSingleplayerWorld(DebugRenderer& debugRenderer) {
	singleplayerWorld = std::make_unique<World>(5, this);
	isLoadedIntoSingleplayerWorld = true;
	singleplayerWorld->Setup();
	debugRenderer.SetupBuffers(singleplayerWorld->GetChunkDebugVisualizationVertices(), singleplayerWorld->GetChunkDebugVisualizationIndices(), singleplayerWorld->GetChunkOrigins());

	EventManager& eventManager = EventManager::GetInstance();
	eventManager.RegisterEvent("event/entity_moved_chunk");
	eventManager.AddEventToDo("event/entity_moved_chunk", [&](Event& event) {
		singleplayerWorld->GenerateChunksAroundPosition(event);
	});
	eventManager.RegisterEvent("event/generate_world");
	eventManager.AddEventToDo("event/generate_world", [&](Event& event) {
		singleplayerWorld->GenerateWorld();
	});
	eventManager.RegisterEvent("event/unload_world");
	eventManager.AddEventToDo("event/unload_world", [&](Event& event) {
		singleplayerWorld->GetChunkHandler().Delete();
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