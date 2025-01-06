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
#include <Texture.h>


struct BlockStringID {
    const char* modID;
    const char* blockName;

    bool IsAir() {
        return strcmp(blockName, "air") == 0;
    }

    bool operator==(const BlockStringID& other) const {
        return strcmp(modID, other.modID) == 0 && strcmp(blockName, other.blockName) == 0;
    }

    std::string CanonicalName() {
        return std::string(modID) + ":" + blockName;
    }
};

template <>
struct std::hash<BlockStringID>{
    std::size_t operator()(const BlockStringID& blockStringID) const {
        using std::size_t;

        return std::hash<std::string>()(std::string(blockStringID.modID) + ":" + blockStringID.blockName);
    }
};

typedef unsigned int TextureID;

enum FaceDirection {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

struct BlockPosition {
    std::bitset<5> x;
    std::bitset<5> y;
    std::bitset<5> z;

    int xInt () const {
        return static_cast<int>(x.to_ulong());
    }

    int yInt () const {
        return static_cast<int>(x.to_ulong());
    }

    int zInt () const {
        return static_cast<int>(x.to_ulong());
    }
};

struct BlockType {
    BlockStringID blockStringID;
    std::vector<TextureID> textures;
    // Textures given by the block manager currently don't mean anything. They are not connected to the actual textures rendered on screen.
    // For that a texture-mesher type thing would be needed like in Minecraft to take all of the seperate block textures, and combine them into one atlas.

    // TODO: When needed, some way to access different blocks properties such as friction, blast resistance, etc.
    // How should this block data be stored? Reading from a generated or pre-made file probably isn't efficient.
    // Could try and register block properties using RegisterBlockType, and have some data fixer-upper fill in empty values.
    // This would have to be stored in some custom data structure with another map and everything though so maybe that isn't the best idea.
    // Also, custom block properties need to be accounted for. If a mod wants to add some custom data to its blocks to be read, how should that be done?

    bool IsAir() {
        return blockStringID.IsAir();
    }
};


class BlockManager {
    public:
        BlockManager(): blockTypes({}) {
            RegisterBlockType({
                .blockStringID = {"kiwicubed", "air"},
                .textures = {0}
            });
            RegisterBlockType({
                .blockStringID = {"kiwicubed", "stone"},
                .textures = {0, 1, 2, 3}
            });
        }

        void RegisterBlockType(BlockType blockType);

        BlockType* GetBlockType(BlockStringID blockStringID);
        BlockType* GetBlockType(const char* modID, const char* blockName);
        BlockType* GetBlockType(unsigned short blockID);

        unsigned short* GetBlockID(BlockStringID blockStringID);
        unsigned short* GetBlockID(const char* modID, const char* blockName);

    private:
        std::unordered_map<unsigned short, BlockType> blockTypes;

        std::unordered_map<unsigned short, BlockStringID> blockIDsToStrings;
        std::unordered_map<BlockStringID, unsigned short> blockStringsToIDs;

        unsigned short latestBlockID = 0;
};


inline BlockManager blockManager = BlockManager();


class Block {
    public:
        Block() : blockPosition(0, 0, 0), blockID(0) {}
        Block(unsigned short blockID) : blockPosition(0, 0, 0), blockID(blockID) {}

        void GenerateBlock(BlockPosition newBlockPosition, int chunkX, int chunkY, int chunkZ, unsigned int chunkSize, bool debug);
        void AddFace(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, FaceDirection faceDirection, int chunkX, int chunkY, int chunkZ, unsigned int chunkSize);

        unsigned int GetBlockID() const;
        void SetBlockID(unsigned short newBlockID);

        bool IsAir();

    private:
        BlockPosition blockPosition;

        unsigned short blockID;
        unsigned short variant;
        //unsigned short blockState;
};
