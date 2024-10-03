#pragma once

#include <GLError.h>
#include <GLAD/glad.h>

#include <World.h>

class SingleplayerHandler {
	public:
		World singleplayerWorld = World();
		Window& window;
		bool isLoadedIntoSingleplayerWorld;

		SingleplayerHandler(Window &newWindow);

		void Setup();

		void StartSingleplayerWorld();
		void EndSingleplayerWorld();

		void Delete();
};