#include "SingleplayerHandler.h"
#include "Events.h"

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

	singleplayerWorld = std::make_unique<World>(3, 2, this);
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
	eventManager.RegisterFunctionToEvent(EVENT_WORLD_PLAYER_MOVE, [=](const EventData& eventData) {
		const EventData moveEventCopy = eventData;
		const EventWorldPlayerMove* moveEvent = moveEventCopy.GetDataStruct<EventWorldPlayerMove>();
		glm::ivec3 oldChunkPosition = glm::ivec3(moveEvent->oldPlayerX / 32, moveEvent->oldPlayerY / 32, moveEvent->oldPlayerZ / 32);
		glm::ivec3 newChunkPosition = glm::ivec3(moveEvent->newPlayerX / 32, moveEvent->newPlayerY / 32, moveEvent->newPlayerZ / 32);
		if (oldChunkPosition == newChunkPosition) {
			return;
		}

		singleplayerWorld->QueueTickTask([this, moveEventCopy]() -> void {
    		singleplayerWorld->RecalculateChunksToLoad(moveEventCopy);
		});
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