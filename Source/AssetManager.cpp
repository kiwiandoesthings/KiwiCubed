#include "AssetManager.h"
#include <cstddef>
#include "AssetDefinitions.h"
#include "Block.h"
#include "Texture.h"


AssetManager::AssetManager() {
    TextureAtlasData empty = TextureAtlasData{};
    RegisterTexture(MetaTexture{AssetStringID{"kiwicubed", "texture/air"}, std::vector<TextureAtlasData>{empty}});
}

void AssetManager::RegisterTextureAtlas(AssetStringID atlasStringID, Texture textureAtlas) {
    OVERRIDE_LOG_NAME("Asset Manager");
    textureAtlases.insert({atlasStringID, textureAtlas});

    INFO("Successfully registered texture atlas with string ID of \"" + atlasStringID.CanonicalName() + "\"");
}

void AssetManager::RegisterTexture(MetaTexture texture) {
    OVERRIDE_LOG_NAME("Asset Manager");
    textureNumericalIDsToStringIDs.insert({latestTextureID, texture.stringID});
    textureStringIDsToNumericalIDs.insert({texture.stringID, latestTextureID});
    AssetManager::atlasDatas.insert({latestTextureID, texture.atlasData});

    latestTextureID++;

    INFO("Successfully registered texture of with string ID of \"" + texture.stringID.CanonicalName() + "\" and numerical id of {" + std::to_string(latestTextureID - 1) + "} with: {" + std::to_string(texture.atlasData.size()) + "} variants");
}

void AssetManager::RegisterShaderProgram(AssetStringID stringID, Shader shaderProgram) {
    OVERRIDE_LOG_NAME("Asset Manager");
    shaderPrograms.insert({stringID, shaderProgram});

    INFO("Successfully registered shader program with string ID of \"" + stringID.CanonicalName() + "\"");
}

void AssetManager::RegisterEntityModel(MetaEntityModel entityModel) {
    OVERRIDE_LOG_NAME("Asset Manager");
    entityModels.insert({entityModel.stringID, entityModel.model});

    INFO("Successfully registered entity model with string ID of \"" + entityModel.stringID.CanonicalName() + "\" with {" + std::to_string(entityModel.model.vertices.size()) + "} vertices and {" + std::to_string(entityModel.model.indices.size()) + "} indices");
}

Texture* AssetManager::GetTextureAtlas(AssetStringID atlasStringID) {
    OVERRIDE_LOG_NAME("Asset Manager");
    auto iterator = textureAtlases.find(atlasStringID);
    if (iterator != textureAtlases.end()) {
        return &iterator->second;
    } else {
        CRITICAL("Tried to get texture atlas with string id \"" + atlasStringID.CanonicalName() + "\" that did not exist, aborting");
        psnip_trap();
    }
    return &iterator->second;
}

AssetStringID* AssetManager::GetStringID(unsigned int numericalID) {
    OVERRIDE_LOG_NAME("Asset Manager");
    auto iterator = textureNumericalIDsToStringIDs.find(numericalID);
    if (iterator != textureNumericalIDsToStringIDs.end()) {
        return &iterator->second;
    } else {
        CRITICAL("Tried to get string ID for texture with numerical ID {" + std::to_string(numericalID) + "} that did not exist, aborting");
        psnip_trap();
    }
    return &iterator->second;
}

unsigned int AssetManager::GetNumericalID(AssetStringID stringID) {
    OVERRIDE_LOG_NAME("Asset Manager");
    auto iterator = textureStringIDsToNumericalIDs.find(stringID);
    if (iterator != textureStringIDsToNumericalIDs.end()) {
        return iterator->second;
    } else {
        CRITICAL("Tried to get numerical ID for texture with string ID \"" + stringID.CanonicalName() + "\" that did not exist, aborting");
        psnip_trap();
    }
    return textureStringIDsToNumericalIDs[stringID];
}

Shader* AssetManager::GetShaderProgram(AssetStringID stringID) {
    OVERRIDE_LOG_NAME("Asset Manager");
    auto iterator = shaderPrograms.find(stringID);
    if (iterator != shaderPrograms.end()) {
        return &iterator->second;
    } else {
        CRITICAL("Tried to get shader program with string id \"" + stringID.CanonicalName() + "\" that did not exist, aborting");
        psnip_trap();
    }
    return &iterator->second;
}

Model* AssetManager::GetEntityModel(AssetStringID stringID) {
    OVERRIDE_LOG_NAME("Asset Manager");
    auto iterator = entityModels.find(stringID);
    if (iterator != entityModels.end()) {
        return &iterator->second;
    } else {
        CRITICAL("Tried to get entity model with string id \"" + stringID.CanonicalName() + "\" that did not exist, aborting");
        psnip_trap();
    }
    return &iterator->second;
}

std::vector<TextureAtlasData>* AssetManager::GetTextureAtlasData(unsigned int numericalID) {
    OVERRIDE_LOG_NAME("Asset Manager");
    auto iterator = atlasDatas.find(numericalID);
    if (iterator != atlasDatas.end()) {
        return &iterator->second;
    } else {
        CRITICAL("Tried to get atlas data for texture with numerical ID {" + std::to_string(numericalID) + "} that did not exist, aborting");
        psnip_trap();
    }
    return &iterator->second;
}

std::vector<TextureAtlasData>* AssetManager::GetTextureAtlasData(AssetStringID stringID) {
    return GetTextureAtlasData(GetNumericalID(stringID));
}

AssetManager::~AssetManager() {}