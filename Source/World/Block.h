#pragma once

#include <GLError.h>
#include <glad/glad.h>

#include <bitset>
#include <iostream>
#include <vector>
#include <unordered_map>

#include <FastNoise.h>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Renderer.h"
#include "Texture.h"
#include "TextureManager.h"

enum FaceDirection {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

struct BlockType {
    // placeholder examples
    int hardness = 1;
    int flamability = 0;

    // Textures given by the block manager currently don't mean anything. They are not connected to the actual textures rendered on screen.
    // For that a texture-mesher type thing would be needed like in Minecraft to take all of the seperate block textures, and combine them into one atlas.

    // TODO: When needed, some way to access different blocks properties such as friction, blast resistance, etc.
    // How should this block data be stored? Reading from a generated or pre-made file probably isn't efficient.
    // Could try and register block properties using RegisterBlockType, and have some data fixer-upper fill in empty values.
    // This would have to be stored in some custom data structure with another map and everything though so maybe that isn't the best idea.
    // Also, custom block properties need to be accounted for. If a mod wants to add some custom data to its blocks to be read, how should that be done?
};

template <>
struct std::hash<BlockType>{
    std::size_t operator()(const BlockType& blockType) const {
        return std::hash<int>()(blockType.hardness) + std::hash<int>()(blockType.flamability);
    }
};


class BlockManager {
    public:
        BlockManager(): blockTypes({}) {}

        void RegisterBlockType(unsigned int numericalID, BlockType blockType);

        BlockType* GetBlockType(TextureStringID blockStringID);
        BlockType* GetBlockType(unsigned short blockID);

    private:
        std::unordered_map<unsigned short, BlockType> blockTypes;
};


inline BlockManager blockManager = BlockManager();


class Block {
    public:
        Block() : blockX(0), blockY(0), blockZ(0), blockID(0) {}
        Block(unsigned short blockID) : blockX(0), blockY(0), blockZ(0), blockID(blockID) {}

        void GenerateBlock(unsigned short blockX, unsigned short blockY, unsigned short blockZ, int chunkX, int chunkY, int chunkZ, unsigned int chunkSize, bool debug);
        void AddFaces(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, std::vector<FaceDirection>* faceDirections, int chunkX, int chunkY, int chunkZ, unsigned int chunkSize, TextureAtlasData& atlasData);

        unsigned int GetBlockID() const;
        void SetBlockID(unsigned int newBlockID);

        unsigned short GetVariant() const;
        void SetVariant(unsigned short newVariant);

        bool IsAir();

    private:
        // Don't need this, look at that one convo, remember chunk palettizing
        unsigned short blockX;
        unsigned short blockY;
        unsigned short blockZ;

        unsigned short blockID;
        unsigned short variant;
        //unsigned short blockState;
};
