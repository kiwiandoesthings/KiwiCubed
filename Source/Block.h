#pragma once

#include <GLError.h>
#include <GLAD/glad.h>

#include <iostream>
#include <vector>

#include <FastNoise/FastNoise.h>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Renderer.h"


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

        void GenerateBlock(int blockX, int blockY, int blockZ, int chunkX, int chunkY, int chunkZ, int chunkSize, FastNoiseLite& noise);
        void AddFace(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, FaceDirection faceDirection, int chunkX, int chunkY, int chunkZ, int chunkSize);
        
        bool GetType();
        void SetType(bool newType);

    private:
        int blockX, blockY, blockZ;

        int type;
};