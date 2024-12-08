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
        Block() {}
        Block(int type);

        void GenerateBlock(unsigned short blockX, unsigned short blockY, unsigned short blockZ, int chunkX, int chunkY, int chunkZ, unsigned int chunkSize, bool debug);
        void AddFace(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, FaceDirection faceDirection, int chunkX, int chunkY, int chunkZ, unsigned short blockX, unsigned short blockY, unsigned short blockZ, unsigned int chunkSize);
        
        unsigned int GetType() const;
        void SetType(unsigned int newType);

    private:
        //unsigned short blockState;
        unsigned short type = 0;
};