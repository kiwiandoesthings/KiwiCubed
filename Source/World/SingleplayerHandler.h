#pragma once

#include <GLError.h>
#include <glad/glad.h>

#include <World.h>

class SingleplayerHandler {
	public:
		World singleplayerWorld;

		bool isLoadedIntoSingleplayerWorld = false;

		void Setup();

		void StartSingleplayerWorld();
		void EndSingleplayerWorld();

		void Delete();

	SingleplayerHandler() : singleplayerWorld(5, this), isLoadedIntoSingleplayerWorld(false) {};
};