#pragma once

#include <glad/glad.h>

#include <World.h>

class SingleplayerHandler {
	public:
		World singlePlayerWorld = World();
		bool isLoadedIntoSingleplayerWorld;

		SingleplayerHandler();
		~SingleplayerHandler();

		void StartSingleplayerWorld();
		void EndSingleplayerWorld();
};