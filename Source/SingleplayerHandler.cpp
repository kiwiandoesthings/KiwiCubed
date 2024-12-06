#include "SingleplayerHandler.h"


SingleplayerHandler::SingleplayerHandler(Window &newWindow) : window(newWindow), isLoadedIntoSingleplayerWorld(false) {
}

void SingleplayerHandler::Setup() {
	singleplayerWorld.Setup(window);
}

void SingleplayerHandler::StartSingleplayerWorld() {
	isLoadedIntoSingleplayerWorld = true;
}

void SingleplayerHandler::EndSingleplayerWorld() {
	isLoadedIntoSingleplayerWorld = false;
}

void SingleplayerHandler::Delete() {

}