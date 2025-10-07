#include "Block.h"
#include "ModHandler.h"


BlockManager& BlockManager::GetInstance() {
    static BlockManager instance;
    return instance;
}

BlockManager::BlockManager() {
    std::vector<MetaTexture> metaTextures;
    std::vector<unsigned char> faceIDs = {0, 0, 0, 0, 0, 0};
    BlockType air = {
        AssetStringID{"kiwicubed", "air"},
        metaTextures,
        faceIDs    
    };
    RegisterBlockType(AssetStringID{"kiwicubed", "air"}, air);
}

void BlockManager::RegisterBlockType(AssetStringID blockStringID, BlockType blockType) {
    OVERRIDE_LOG_NAME("Block Type Registration");
    
    numericalIDsToStringIDs.insert({lastNumericalID, blockStringID});
    stringIDsToNumericalIDs.insert({blockStringID, lastNumericalID});
    numericalIDsToBlockTypes.insert({lastNumericalID, blockType});
    stringIDsToBlockTypes.insert({blockStringID, blockType});

    INFO("Successfully registered block type with numerical ID of {" + std::to_string(lastNumericalID) + "} and string ID of \"" + blockStringID.CanonicalName() + "\"");
    
    lastNumericalID++;
}

AssetStringID* BlockManager::GetStringID(unsigned short numericalID) {
    return &numericalIDsToStringIDs[numericalID];
}

unsigned short BlockManager::GetNumericalID(AssetStringID blockStringID) {
    return stringIDsToNumericalIDs[blockStringID];
}

BlockType* BlockManager::GetBlockType(unsigned short numericalID) {
    return &numericalIDsToBlockTypes[numericalID];
}

BlockType* BlockManager::GetBlockType(AssetStringID blockStringID) {
    return &stringIDsToBlockTypes[blockStringID];
}