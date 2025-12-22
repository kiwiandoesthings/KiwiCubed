#include "SingleplayerHandler.h"
#include "Events.h"
#include <cstddef>

SingleplayerHandler::SingleplayerHandler() : singleplayerWorld(nullptr), isLoadedIntoSingleplayerWorld(false) {
}

void SingleplayerHandler::Setup(DebugRenderer& debugRenderer) {
	EventManager& eventManager = EventManager::GetInstance();

	SingleplayerHandler* self = this;
	eventManager.RegisterEvent("event/generate_world");
	eventManager.AddEventToDo("event/generate_world", [self, &debugRenderer](Event& event) {
		OVERRIDE_LOG_NAME("Singleplayer Handler");
		if (self->singleplayerWorld != nullptr) {
			CRITICAL("Tried to generate already generated world, aborting");
			psnip_trap();
		}

		self->singleplayerWorld = std::make_unique<World>(5, 3, self);
		self->isLoadedIntoSingleplayerWorld = true;
		self->StartSingleplayerWorld(debugRenderer);
		self->singleplayerWorld->GenerateWorld();
		self->singleplayerWorld->Setup();
		self->isLoadedIntoSingleplayerWorld = true;
		debugRenderer.SetupBuffers(self->singleplayerWorld->GetChunkDebugVisualizationVertices(), self->singleplayerWorld->GetChunkDebugVisualizationIndices(), self->singleplayerWorld->GetChunkOrigins());

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
	Update();
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

World* SingleplayerHandler::GetWorld() {
	return singleplayerWorld.get();
}

bool SingleplayerHandler::IsLoadedIntoSingleplayerWorld() {
	return isLoadedIntoSingleplayerWorld;
}

void SingleplayerHandler::Delete() {
	EndSingleplayerWorld();
}