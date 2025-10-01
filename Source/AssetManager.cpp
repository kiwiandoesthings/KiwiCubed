#include "AssetManager.h"
#include <cstddef>
#include "Block.h"


AssetManager::AssetManager() {}

void AssetManager::RegisterTexture(MetaTexture texture) {
    OVERRIDE_LOG_NAME("Texture Manager");
    numericalIDsToStringIDs.insert({latestTextureID + 1, texture.stringID});
    stringIDsToNumericalIDs.insert({texture.stringID, latestTextureID + 1});
    AssetManager::atlasData.insert({latestTextureID + 1, texture.atlasData});

    latestTextureID++;

    INFO("Successfully registered texture of with string ID of \"" + texture.stringID.CanonicalName() + "\" and numerical id of {" + std::to_string(latestTextureID) + "} with: {" + std::to_string(texture.atlasData.size()) + "} variants");
}

AssetStringID* AssetManager::GetStringID(unsigned int numericalID) {
    OVERRIDE_LOG_NAME("Texture Manager");
    auto iterator = numericalIDsToStringIDs.find(numericalID);
    if (iterator != numericalIDsToStringIDs.end()) {
        return &iterator->second;
    } else {
        ERR("Tried to get string ID for texture with numerical ID {" + std::to_string(numericalID) + "} that did not exist, aborting");
        psnip_trap();
    }
    return &iterator->second;
}

unsigned int AssetManager::GetNumericalID(AssetStringID stringID) {
    OVERRIDE_LOG_NAME("Texture Manager");
    auto iterator = stringIDsToNumericalIDs.find(stringID);
    if (iterator != stringIDsToNumericalIDs.end()) {
        return iterator->second;
    } else {
        ERR("Tried to get numerical ID for texture with string ID \"" + stringID.CanonicalName() + "\" that did not exist, aborting");
        psnip_trap();
    }
    return stringIDsToNumericalIDs[stringID];
}

std::vector<TextureAtlasData>* AssetManager::GetTextureAtlasData(unsigned int numericalID) {
    OVERRIDE_LOG_NAME("Texture Manager");
    auto iterator = atlasData.find(numericalID);
    if (iterator != atlasData.end()) {
        return &iterator->second;
    } else {
        ERR("Tried to get atlas data for texture with numerical ID \"" + std::to_string(numericalID) + "\" that did not exist, aborting");
        psnip_trap();
    }
    return &iterator->second;
}

std::vector<TextureAtlasData>* AssetManager::GetTextureAtlasData(AssetStringID stringID) {
    return GetTextureAtlasData(GetNumericalID(stringID));
}

AssetManager::~AssetManager() {}