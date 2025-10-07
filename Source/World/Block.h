#pragma once

#include <GLError.h>
#include <glad/glad.h>
#include <debug-trap.h>

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

#include "ModHandler.h"
#include "Renderer.h"
#include "Texture.h"
#include "AssetManager.h"


enum FaceDirection {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

struct BlockModel {
    inline static GLfloat faceVertices[120] = {
        // Positions       // Texture Coordinates
        // Front
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,  0.0f, 1.0f,

        // Back
        1.0f, 0.0f, 1.0f,  0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        0.0f, 1.0f, 1.0f,  1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,  0.0f, 1.0f,

        // Left
        0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,  0.0f, 1.0f,

        // Right
        1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,  0.0f, 1.0f,

        // Top
        0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,  0.0f, 1.0f,

        // Bottom
        0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
        1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        0.0f, 0.0f, 0.0f,  0.0f, 1.0f
    };

    inline static GLuint faceIndices[36] = {
        // Front
        0, 1, 2,
        2, 3, 0,

        // Back
        4, 5, 6,
        6, 7, 4,

        // Left
        8, 9, 10,
        10, 11, 8,

        // Right
        12, 13, 14,
        14, 15, 12,

        // Top
        16, 17, 18,
        18, 19, 16,

        // Bottom
        20, 21, 22,
        22, 23, 20
    };
};


struct BlockType {
    AssetStringID blockStringID;

    std::vector<MetaTexture> metaTextures;
    unsigned char frontFaceID;
    unsigned char backFaceID;
    unsigned char leftFaceID;
    unsigned char rightFaceID;
    unsigned char topFaceID;
    unsigned char bottomFaceID;

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

    BlockType() : blockStringID(AssetStringID{"kiwicubed", "invalid"}) {}
    // world's longest constructor
    BlockType(AssetStringID blockStringID, std::vector<MetaTexture> metaTextures, std::vector<unsigned char> faceTextureIDs) : blockStringID(blockStringID), metaTextures(metaTextures) {
        if (faceTextureIDs.size() != 6) {
            CRITICAL("Tried to create block type with wrong amount of faceTextureIDs, aborting");
            psnip_trap();
        }

        frontFaceID = faceTextureIDs[0];
        backFaceID = faceTextureIDs[1];
        leftFaceID = faceTextureIDs[2];
        rightFaceID = faceTextureIDs[3];
        topFaceID = faceTextureIDs[4];
        bottomFaceID = faceTextureIDs[5];
    }
};

template <>
struct std::hash<BlockType>{
    std::size_t operator()(const BlockType& blockType) const {
        return std::hash<std::string>()(blockType.blockStringID.CanonicalName());
    }
};


class BlockManager {
    public:
        static BlockManager& GetInstance();

        BlockManager();

        void RegisterBlockType(AssetStringID blockStringID, BlockType blockType);

        AssetStringID* GetStringID(unsigned short numericalID);
        unsigned short GetNumericalID(AssetStringID);
        BlockType* GetBlockType(unsigned short numericalID);
        BlockType* GetBlockType(AssetStringID blockStringID);

    private:
        ~BlockManager() = default;

        BlockManager(const BlockManager&) = delete;
        BlockManager& operator=(const BlockManager&) = delete;

        std::unordered_map<unsigned short, AssetStringID> numericalIDsToStringIDs;
        std::unordered_map<AssetStringID, unsigned short> stringIDsToNumericalIDs;
        std::unordered_map<unsigned short, BlockType> numericalIDsToBlockTypes;
        std::unordered_map<AssetStringID, BlockType> stringIDsToBlockTypes;

        unsigned short lastNumericalID = 0;
};


class Block {
    public:
        // Don't need this, look at that one convo, remember chunk palettizing
        unsigned short blockID;
        unsigned char variant;
        unsigned char blockState;

        Block() : blockID(1), variant(0) {}
        Block(unsigned short blockID, unsigned short variant) : blockID(blockID), variant(variant) {}

        unsigned int GetBlockID() const;
        void SetBlockID(unsigned int newBlockID);

        unsigned short GetVariant() const;
        void SetVariant(unsigned short newVariant);

        bool IsAir();
};