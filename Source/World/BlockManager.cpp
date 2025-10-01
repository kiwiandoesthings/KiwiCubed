#include "Block.h"
#include "ModHandler.h"


void BlockManager::RegisterBlockType(AssetStringID blockStringID, BlockType blockType) {
    OVERRIDE_LOG_NAME("Block Type Registration");
    blockTypes.insert({blockStringID, blockType});

    INFO("Successfully registered block type with string ID of \"" + blockStringID.CanonicalName() + "\"");
}

BlockType* BlockManager::GetBlockType(AssetStringID blockStringID) {
    return &blockTypes[blockStringID];
}