#include "SingleplayerHandler.h"
#include "Events.h"
#include <cstddef>

SingleplayerHandler::SingleplayerHandler(DebugRenderer& debugRenderer) : singleplayerWorld(nullptr), isLoadedIntoSingleplayerWorld(false), debugRenderer(debugRenderer) {
}

void SingleplayerHandler::Setup() {
}

void SingleplayerHandler::StartSingleplayerWorld() {
	OVERRIDE_LOG_NAME("Singleplayer Handler");
	if (singleplayerWorld != nullptr) {
		CRITICAL("Tried to generate already generated world, aborting");
		psnip_trap();
	}

	singleplayerWorld = std::make_unique<World>(5, 3, this);
	isLoadedIntoSingleplayerWorld = true;
	singleplayerWorld->GenerateWorld();
	singleplayerWorld->Setup();
	isLoadedIntoSingleplayerWorld = true;
	debugRenderer.SetupBuffers(singleplayerWorld->GetChunkDebugVisualizationVertices(), singleplayerWorld->GetChunkDebugVisualizationIndices(), singleplayerWorld->GetChunkOrigins());

	if (UI::GetInstance().GetCurrentScreenName() == "ui/main_menu") {
		UI::GetInstance().DisableUI();
	}

	Physics::Initialize();

	EventManager& eventManager = EventManager::GetInstance();
	eventManager.RegisterFunctionToEvent(EVENT_WORLD_PLAYER_MOVE, [&](EventData& eventData) {
		singleplayerWorld->QueueTickTask([&, this] {
			singleplayerWorld->RecalculateChunksToLoad(eventData);
		});
	});
	eventManager.RegisterFunctionToEvent(EVENT_WORLD_PLAYER_BLOCK_EVENT, [&](EventData& eventData) {
		WorldPlayerBlockEvent* blockEvent = static_cast<WorldPlayerBlockEvent*>(eventData.data);
		std::cout << blockEvent->blockX << std::endl;
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
		UI& ui = UI::GetInstance();
		ui.SetCurrentScreen("ui/main_menu");
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