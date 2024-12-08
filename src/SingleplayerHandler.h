#pragma once

#include <GLError.h>
#include <glad/glad.h>

#include <World.h>

class SingleplayerHandler {
	public:
		World singleplayerWorld;
		Window& window;

		bool isLoadedIntoSingleplayerWorld = false;

		void Setup();

		void StartSingleplayerWorld();
		void EndSingleplayerWorld();

		void Delete();

	SingleplayerHandler(Window &newWindow) : singleplayerWorld(5, this), window(newWindow), isLoadedIntoSingleplayerWorld(false) {};
};