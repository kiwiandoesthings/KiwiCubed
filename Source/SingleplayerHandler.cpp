#include "SingleplayerHandler.h"

void SingleplayerHandler::Setup() { singleplayerWorld.Setup(window); }

void SingleplayerHandler::StartSingleplayerWorld() {
    OVERRIDE_LOG_NAME("Singleplayer World Start");
    isLoadedIntoSingleplayerWorld = true;
    INFO("Starting singleplayer world");
    singleplayerWorld.StartTickThread();
}

void SingleplayerHandler::EndSingleplayerWorld() {
    OVERRIDE_LOG_NAME("World Exit");
    isLoadedIntoSingleplayerWorld = false;
    INFO("Exiting singleplayer world");
    singleplayerWorld.StopTickThread();
}

void SingleplayerHandler::Delete() { EndSingleplayerWorld(); }
