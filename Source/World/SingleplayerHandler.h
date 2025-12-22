#pragma once

#include <GLError.h>
#include <glad/glad.h>

#include "DebugRenderer.h"
#include "Physics.h"
#include "UI.h"
#include "World.h"


class SingleplayerHandler {
	public:
		SingleplayerHandler();
		
		void Setup(DebugRenderer& debugRenderer);
		void StartSingleplayerWorld(DebugRenderer& debugRenderer);
		void EndSingleplayerWorld();

		void Update();

		World* GetWorld();
		bool IsLoadedIntoSingleplayerWorld();

		void Delete();
	
	private:
		std::unique_ptr<World> singleplayerWorld = nullptr;
		bool isLoadedIntoSingleplayerWorld = false;
		bool shouldUnloadWorld = false;
};