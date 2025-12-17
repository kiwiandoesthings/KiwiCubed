#include "SingleplayerHandler.h"
#include "Events.h"
#include <cstddef>

SingleplayerHandler::SingleplayerHandler(DebugRenderer& debugRenderer) : singleplayerWorld(nullptr), isLoadedIntoSingleplayerWorld(false) {
	EventManager& eventManager = EventManager::GetInstance();
	eventManager.RegisterEvent("event/generate_world");
	eventManager.AddEventToDo("event/generate_world", [&](Event& event) {
		singleplayerWorld = std::make_unique<World>(5, 3, this);
		isLoadedIntoSingleplayerWorld = true;
		StartSingleplayerWorld(debugRenderer);
		singleplayerWorld->GenerateWorld();
		singleplayerWorld->Setup();
		debugRenderer.SetupBuffers(singleplayerWorld->GetChunkDebugVisualizationVertices(), singleplayerWorld->GetChunkDebugVisualizationIndices(), singleplayerWorld->GetChunkOrigins());

		if (UI::GetInstance().GetCurrentScreenName() == "ui/main_menu") {
			UI::GetInstance().DisableUI();
		}
	});
}

void SingleplayerHandler::StartSingleplayerWorld(DebugRenderer& debugRenderer) {
	Physics::Initialize();

	EventManager& eventManager = EventManager::GetInstance();
	eventManager.RegisterEvent("event/unload_world");
	eventManager.AddEventToDo("event/unload_world", [&](Event& event) {
		shouldUnloadWorld = true;
	});
	eventManager.RegisterEvent("event/player_moved_chunk");
	eventManager.AddEventToDo("event/player_moved_chunk", [&](Event& event) {
		singleplayerWorld->QueueTickTask([=, this] {
			singleplayerWorld->RecalculateChunksToLoad(event);
		});
	});
	eventManager.RegisterEvent("event/player_mined_block");
	eventManager.AddEventToDo("event/player_mined_block", [&](Event& event) {
		auto* chunkPosition = event.GetData<glm::ivec3>("chunkPosition");
		auto* blockPosition = event.GetData<glm::ivec3>("blockPosition");
		auto* blockType = event.GetData<BlockType*>("blockType");
		singleplayerWorld->SpawnItemFromBlock(*chunkPosition, *blockPosition, *blockType);
	});
	
	singleplayerWorld->StartTickThread();
}

void SingleplayerHandler::EndSingleplayerWorld() {
	shouldUnloadWorld = true;
}

void SingleplayerHandler::Update() {
	OVERRIDE_LOG_NAME("Singleplayer Handler");
	if (shouldUnloadWorld) {
		EventManager& eventManager = EventManager::GetInstance();
		singleplayerWorld->Delete();
		singleplayerWorld.reset();
		isLoadedIntoSingleplayerWorld = false;
		eventManager.TriggerEvent("ui/move_screen_main_menu");
		eventManager.DeregisterEvent("event/unload_world");
		eventManager.DeregisterEvent("event/player_moved_chunk");
		shouldUnloadWorld = false;
		INFO("Exiting singleplayer world");
	}
}

void SingleplayerHandler::Delete() {
	EndSingleplayerWorld();
}