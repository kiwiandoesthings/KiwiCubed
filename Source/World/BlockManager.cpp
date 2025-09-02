#include "Block.h"


void BlockManager::RegisterBlockType(unsigned int numericalID, BlockType blockType) {
    OVERRIDE_LOG_NAME("Block Type Registration");
    blockTypes.insert({numericalID, blockType});

    INFO("Successfully registered block type with numerical ID of {" + std::to_string(numericalID) + "}");
}

BlockType* BlockManager::GetBlockType(TextureStringID blockStringID) {
    return &blockTypes[static_cast<unsigned short>(*textureManager.GetNumericalID(blockStringID))];
}

BlockType* BlockManager::GetBlockType(unsigned short blockID) {
    return &blockTypes[blockID];
}