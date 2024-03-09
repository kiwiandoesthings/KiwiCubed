#pragma once

#include <GLError.h>
#include <GLAD/glad.h>

#include <World.h>

class SingleplayerHandler {
	public:
		World singlePlayerWorld = World();
		bool isLoadedIntoSingleplayerWorld;

		SingleplayerHandler();

		void StartSingleplayerWorld();
		void EndSingleplayerWorld();

		void Delete();
};