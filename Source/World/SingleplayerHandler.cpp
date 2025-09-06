#include "SingleplayerHandler.h"
#include "Events.h"
#include <cstddef>

SingleplayerHandler::SingleplayerHandler(DebugRenderer& debugRenderer) : singleplayerWorld(nullptr), isLoadedIntoSingleplayerWorld(false) {
	EventManager& eventManager = EventManager::GetInstance();
	eventManager.RegisterEvent("event/generate_world");
	eventManager.AddEventToDo("event/generate_world", [&](Event& event) {
		singleplayerWorld = std::make_unique<World>(5, 3, this);
		isLoadedIntoSingleplayerWorld = true;
		singleplayerWorld->GenerateWorld();
		singleplayerWorld->Setup();
		debugRenderer.SetupBuffers(singleplayerWorld->GetChunkDebugVisualizationVertices(), singleplayerWorld->GetChunkDebugVisualizationIndices(), singleplayerWorld->GetChunkOrigins());
		StartSingleplayerWorld(debugRenderer);

		if (UI::GetInstance().GetCurrentScreenName() == "ui/main_menu") {
			UI::GetInstance().DisableUI();
		}
	});
}

void SingleplayerHandler::StartSingleplayerWorld(DebugRenderer& debugRenderer) {
	EventManager& eventManager = EventManager::GetInstance();
	eventManager.RegisterEvent("event/unload_world");
	eventManager.AddEventToDo("event/unload_world", [&](Event& event) {
		shouldUnloadWorld = true;
	});
	eventManager.RegisterEvent("event/entity_moved_chunk");
	eventManager.AddEventToDo("event/entity_moved_chunk", [&](Event& event) {
		singleplayerWorld->GenerateChunksAroundPosition(event, 5, 2);
	});
	
	singleplayerWorld->StartTickThread();
}

void SingleplayerHandler::EndSingleplayerWorld() {
	OVERRIDE_LOG_NAME("Singleplayer Handler");
	isLoadedIntoSingleplayerWorld = false;
	singleplayerWorld->Delete();
	INFO("Exiting singleplayer world");
}

void SingleplayerHandler::Update() {
	EventManager& eventManager = EventManager::GetInstance();
	if (shouldUnloadWorld) {
		singleplayerWorld->Delete();
		singleplayerWorld.reset();
		isLoadedIntoSingleplayerWorld = false;
		eventManager.TriggerEvent("ui/move_screen_main_menu");
		shouldUnloadWorld = false;
	}
}

void SingleplayerHandler::Delete() {
	EndSingleplayerWorld();
}