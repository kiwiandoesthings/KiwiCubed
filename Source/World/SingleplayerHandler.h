#pragma once

#include <GLError.h>
#include <glad/glad.h>

#include "DebugRenderer.h"
#include "Physics.h"
#include "World.h"


class SingleplayerHandler {
	public:
		std::unique_ptr<World> singleplayerWorld = nullptr;
		bool isLoadedIntoSingleplayerWorld = false;
		
		SingleplayerHandler(DebugRenderer& debugRenderer);
		
		void StartSingleplayerWorld(DebugRenderer& debugRenderer);
		void EndSingleplayerWorld();

		void Update();

		void Delete();
	
	private:
		bool shouldUnloadWorld = false;
};