#pragma once

#include <GLError.h>
#include <glad/glad.h>

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


struct BlockID {
    const char* modID;
    const char* blockName;

    bool isAir() {
        return strcmp(blockName, "air") == 0;
    }

    bool operator==(const BlockID& other) const {
        return strcmp(modID, other.modID) == 0 && strcmp(blockName, other.blockName) == 0;
    }

    std::string canonicalName() {
        return std::string(modID) + ":" + blockName;
    }
};

template <>
struct std::hash<BlockID>{
    std::size_t operator()(const BlockID& k) const {
        using std::size_t;

        return std::hash<std::string>()(std::string(k.modID) + ":" + k.blockName);
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

struct BlockPos {
    unsigned short x;
    unsigned short y;
    unsigned short z;
};

struct BlockType {
    BlockID blockID;
    std::vector<TextureID> textures;

    bool isAir() {
        return textures[0] == 0;
    }
};

class BlockManager {
    public:
        BlockManager(): blockTypes({}) {
            registerBlockType({
                .blockID = {"kiwicubed", "air"},
                .textures = {0}
            });
            registerBlockType({
                .blockID = {"kiwicubed", "stone"},
                .textures = {1, 2, 3, 4}
            });
        }

        void registerBlockType(BlockType blockType) {
            blockTypes[blockType.blockID] = blockType;
        }

        BlockType* GetBlockType(BlockID blockID) {
            return &blockTypes[blockID];
        }

        BlockType* GetBlockType(const char* modID, const char* blockName) {
            return &blockTypes[{modID, blockName}];
        }

    private:
        std::unordered_map<BlockID, BlockType> blockTypes;
};

inline BlockManager gBlockManager = BlockManager();


class Block {
    public:
        Block() : pos({0,0,0}), type(gBlockManager.GetBlockType("kiwicubed", "air")) {}
        Block(BlockType* type) : pos({0,0,0}), type(type) {}

        void GenerateBlock(BlockPos newpos, int chunkX, int chunkY, int chunkZ, unsigned int chunkSize, bool debug);
        void AddFace(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, FaceDirection faceDirection, int chunkX, int chunkY, int chunkZ, unsigned int chunkSize);

        void SetType(BlockType* newBlockType) {
            type = newBlockType;
        }

        BlockType* GetType() {
            return type;
        }

    private:
        BlockPos pos;

        //unsigned short blockState;
        unsigned short variant;
        BlockType* type;
};
