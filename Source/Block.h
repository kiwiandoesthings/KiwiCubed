#pragma once

#include <GLError.h>
#include <GLAD/glad.h>

#include <iostream>
#include <vector>

#include <FastNoise/FastNoise.h>
#include <glm/vec3.hpp>


enum FaceDirection {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};


class Block {
    public:
        Block() : blockX(0), blockY(0), blockZ(0), type(NULL) {}
        Block(int type);

        void GenerateBlock(int blockX, int blockY, int blockZ, int chunkX, int chunkY, int chunkZ, int chunkSize);
        void AddFace(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, FaceDirection faceDirection, int chunkX, int chunkY, int chunkZ, int chunkSize);
        
        bool GetType();
        void SetType(bool newType);

    private:
        int blockX, blockY, blockZ;

        bool type;
};