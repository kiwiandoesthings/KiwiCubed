#pragma once

#include "Block.h"


void BlockManager::RegisterBlockType(BlockType blockType) {
    blockTypes.insert({latestBlockID + 1, blockType});
    blockIDsToStrings.insert({latestBlockID + 1, blockType.blockStringID});
    blockStringsToIDs.insert({blockType.blockStringID, latestBlockID + 1});

    latestBlockID++;
}

BlockType* BlockManager::GetBlockType(BlockStringID blockStringID) {
    return &blockTypes[*GetBlockID(blockStringID)];
}

BlockType* BlockManager::GetBlockType(const char* modID, const char* blockName) {
    return &blockTypes[*GetBlockID(modID, blockName)];
}

BlockType* BlockManager::GetBlockType(unsigned short blockID) {
    return &blockTypes[blockID];
}

unsigned short* BlockManager::GetBlockID(BlockStringID blockStringID) {
    return &blockStringsToIDs[blockStringID];
}

unsigned short* BlockManager::GetBlockID(const char* modID, const char* blockName) {
    return &blockStringsToIDs[{modID, blockName}];
}