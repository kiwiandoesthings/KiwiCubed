#pragma once

#include <GLError.h>
#include <glad/glad.h>

#include <DebugRenderer.h>
#include <World.h>

class SingleplayerHandler {
	public:
		std::unique_ptr<World> singleplayerWorld = nullptr;
		bool isLoadedIntoSingleplayerWorld = false;
		
		SingleplayerHandler() : singleplayerWorld(nullptr), isLoadedIntoSingleplayerWorld(false) {};
		
		void StartSingleplayerWorld(DebugRenderer& debugRenderer);
		void EndSingleplayerWorld();

		void Delete();

};