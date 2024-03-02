#pragma once

#include <glad/glad.h>

#include <World.h>

class SingleplayerHandler {
	public:
		SingleplayerHandler();
		~SingleplayerHandler();

		void StartSingleplayerWorld();
		void EndSingleplayerWorld();

		World singlePlayerWorld = World();
		bool isLoadedIntoSingleplayerWorld;
};