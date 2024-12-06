#pragma once

#include <GLError.h>
#include <GLAD/glad.h>

#include <World.h>

class SingleplayerHandler {
	public:
		World singleplayerWorld = World(5, *this);
		Window& window;

		bool isLoadedIntoSingleplayerWorld = false;

		SingleplayerHandler(Window &newWindow);

		void Setup();

		void StartSingleplayerWorld();
		void EndSingleplayerWorld();

		void Delete();
};