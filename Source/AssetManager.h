#pragma once

#include <klogger.hpp>
#include <debug-trap.h>

#include <string>
#include <unordered_map>

#include "AssetDefinitions.h"
#include "ModHandler.h"
#include "Texture.h"


class AssetManager {
    public:
        AssetManager();
        ~AssetManager();

        void RegisterTextureAtlas(AssetStringID atlasStringID, Texture textureAtlas);
        void RegisterTexture(MetaTexture texture);
        void RegisterShaderProgram(AssetStringID stringID, Shader shaderProgram);
        void RegisterEntityModel(MetaEntityModel entityModel);

        Texture* GetTextureAtlas(AssetStringID atlasStringID);

        AssetStringID* GetStringID(unsigned int numericalID);
        unsigned int GetNumericalID(AssetStringID stringID);

        Shader* GetShaderProgram(AssetStringID stringID);

        Model* GetEntityModel(AssetStringID stringID);

        std::vector<TextureAtlasData>* GetTextureAtlasData(unsigned int numericalID);
        std::vector<TextureAtlasData>* GetTextureAtlasData(AssetStringID stringID);

        AssetStringID StringToAssetStruct(std::string stringID, std::string assetPrefix);

    private:
        robin_hood::unordered_flat_map<AssetStringID, Texture> textureAtlases;

        robin_hood::unordered_flat_map<unsigned int, AssetStringID> textureNumericalIDsToStringIDs;
        robin_hood::unordered_flat_map<AssetStringID, unsigned int> textureStringIDsToNumericalIDs;

        robin_hood::unordered_flat_map<unsigned int, std::vector<TextureAtlasData>> atlasDatas;

        robin_hood::unordered_flat_map<AssetStringID, Shader> shaderPrograms;

        robin_hood::unordered_flat_map<AssetStringID, Model> entityModels;

        int latestTextureID = 0;
};

inline AssetManager assetManager = AssetManager();