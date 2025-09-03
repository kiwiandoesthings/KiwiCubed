#include "SingleplayerHandler.h"


SingleplayerHandler::SingleplayerHandler(DebugRenderer& debugRenderer) : singleplayerWorld(nullptr), isLoadedIntoSingleplayerWorld(false) {
	EventManager& eventManager = EventManager::GetInstance();
	eventManager.RegisterEvent("event/generate_world");
	eventManager.AddEventToDo("event/generate_world", [&](Event& event) {
		singleplayerWorld = std::make_unique<World>(5, this);
		isLoadedIntoSingleplayerWorld = true;
		singleplayerWorld->GenerateWorld();
		singleplayerWorld->Setup();
		debugRenderer.SetupBuffers(singleplayerWorld->GetChunkDebugVisualizationVertices(), singleplayerWorld->GetChunkDebugVisualizationIndices(), singleplayerWorld->GetChunkOrigins());
		StartSingleplayerWorld(debugRenderer);
	});
}

void SingleplayerHandler::StartSingleplayerWorld(DebugRenderer& debugRenderer) {
	EventManager& eventManager = EventManager::GetInstance();
	eventManager.RegisterEvent("event/unload_world");
	eventManager.AddEventToDo("event/unload_world", [&](Event& event) {
		singleplayerWorld->GetChunkHandler().Delete();
	});
	eventManager.RegisterEvent("event/entity_moved_chunk");
	eventManager.AddEventToDo("event/entity_moved_chunk", [&](Event& event) {
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