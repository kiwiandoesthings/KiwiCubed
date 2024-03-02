#include <SingleplayerHandler.h>


SingleplayerHandler::SingleplayerHandler() {

}

void SingleplayerHandler::StartSingleplayerWorld() {
	isLoadedIntoSingleplayerWorld = true;
}

void SingleplayerHandler::EndSingleplayerWorld() {
	isLoadedIntoSingleplayerWorld = false;
}

SingleplayerHandler::~SingleplayerHandler() {

}