#include "SingleplayerHandler.h"


SingleplayerHandler::SingleplayerHandler() : isLoadedIntoSingleplayerWorld(false) {

}

void SingleplayerHandler::StartSingleplayerWorld() {
	isLoadedIntoSingleplayerWorld = true;
}

void SingleplayerHandler::EndSingleplayerWorld() {
	isLoadedIntoSingleplayerWorld = false;
}

void SingleplayerHandler::Delete() {

}