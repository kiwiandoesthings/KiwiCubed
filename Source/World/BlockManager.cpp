#include "Block.h"
#include "ModHandler.h"


BlockManager& BlockManager::GetInstance() {
    static BlockManager instance;
    return instance;
}

BlockManager::BlockManager() {
    std::vector<TextureAtlasData> atlasDatas = {TextureAtlasData{}};
    MetaTexture metaTexture = MetaTexture{AssetStringID{"kiwicubed", "texture/air"}, atlasDatas};
    std::vector<MetaTexture> metaTextures = std::vector<MetaTexture>{metaTexture};
    std::vector<unsigned char> faceIDs = {0, 0, 0, 0, 0, 0};
    BlockType air = {
        AssetStringID{"kiwicubed", "block/air"},
        metaTextures,
        faceIDs    
    };
    RegisterBlockType(air);
}

void BlockManager::RegisterBlockType(BlockType blockType) {
    OVERRIDE_LOG_NAME("Block Type Registration");
    
    numericalIDsToStringIDs.insert({lastNumericalID, blockType.blockStringID});
    stringIDsToNumericalIDs.insert({blockType.blockStringID, lastNumericalID});
    numericalIDsToBlockTypes.insert({lastNumericalID, blockType});
    stringIDsToBlockTypes.insert({blockType.blockStringID, blockType});

    INFO("Successfully registered block type with numerical ID of {" + std::to_string(lastNumericalID) + "} and string ID of \"" + blockType.blockStringID.CanonicalName() + "\"");
    
    lastNumericalID++;
}

AssetStringID* BlockManager::GetStringID(unsigned short numericalID) {
    OVERRIDE_LOG_NAME("Block Manager");
    auto iterator = numericalIDsToStringIDs.find(numericalID);
    if (iterator != numericalIDsToStringIDs.end()) {
        return &iterator->second;
    }
    CRITICAL("Tried to get block string id from numerical id of {" + std::to_string(numericalID) + "} that didn't exist, aborting");
    psnip_trap();
    return nullptr;
}

unsigned short* BlockManager::GetNumericalID(AssetStringID blockStringID) {
    OVERRIDE_LOG_NAME("Block Manager");
    auto iterator = stringIDsToNumericalIDs.find(blockStringID);
    if (iterator != stringIDsToNumericalIDs.end()) {
        return &iterator->second;
    }
    CRITICAL("Tried to get block numerical id from string id of \"" + blockStringID.CanonicalName() + "\" that didn't exist, aborting");
    psnip_trap();
    return nullptr;
}

BlockType* BlockManager::GetBlockType(unsigned short numericalID) {
    OVERRIDE_LOG_NAME("Block Manager");
    auto iterator = numericalIDsToBlockTypes.find(numericalID);
    if (iterator != numericalIDsToBlockTypes.end()) {
        return &iterator->second;
    }
    CRITICAL("Tried to get block type with numerical id of {" + std::to_string(numericalID) + "} that didn't exist, aborting");
    psnip_trap();
    return nullptr;
}

BlockType* BlockManager::GetBlockType(AssetStringID blockStringID) {
    OVERRIDE_LOG_NAME("Block Manager");
    auto iterator = stringIDsToBlockTypes.find(blockStringID);
    if (iterator != stringIDsToBlockTypes.end()) {
        return &iterator->second;
    }
    CRITICAL("Tried to get block type with string id of \"" + blockStringID.CanonicalName() + "\" that didn't exist, aborting");
    psnip_trap();
    return nullptr;
}