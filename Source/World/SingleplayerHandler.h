#pragma once

#include <GLError.h>
#include <glad/glad.h>

#include "DebugRenderer.h"
#include "Physics.h"
#include "UI.h"
#include "World.h"


class SingleplayerHandler {
	public:
		static SingleplayerHandler& GetInstance();
        static void Delete();
		
		void Setup(DebugRenderer* debugRenderer);
		void StartSingleplayerWorld();
		void EndSingleplayerWorld();

		void Update();

		World* GetWorld();
		bool IsLoadedIntoSingleplayerWorld();
	
	private:
		SingleplayerHandler() = default;
        ~SingleplayerHandler() = default;

        SingleplayerHandler(const SingleplayerHandler&) = delete;
        SingleplayerHandler& operator=(const SingleplayerHandler&) = delete;

		std::unique_ptr<World> singleplayerWorld = nullptr;
		DebugRenderer* debugRenderer;
		bool isLoadedIntoSingleplayerWorld = false;
		bool shouldUnloadWorld = false;
};