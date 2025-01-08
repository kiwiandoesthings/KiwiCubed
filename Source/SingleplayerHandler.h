#pragma once

#include <GLError.h>
#include <World.h>
#include <glad/glad.h>

class SingleplayerHandler {
  public:
    World singleplayerWorld;
    Window &window;

    bool isLoadedIntoSingleplayerWorld = false;

    void Setup();

    void StartSingleplayerWorld();
    void EndSingleplayerWorld();

    void Delete();

    SingleplayerHandler(Window &newWindow) : singleplayerWorld(5, this), window(newWindow) {};
};
