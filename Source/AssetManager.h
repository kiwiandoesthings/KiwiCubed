#pragma once

#include <klogger.hpp>
#include <debug-trap.h>

#include <string>
#include <unordered_map>

#include "ModHandler.h"
#include "Texture.h"


struct TextureAtlasData {
    const unsigned short variant = 0;
    const unsigned char xPosition = 0;
    const unsigned char yPosition = 0;
    const unsigned char xSize = 0;
    const unsigned char ySize = 0;

    TextureAtlasData() : variant(0), xPosition(0), yPosition(0), xSize(0), ySize(0) {}
    TextureAtlasData(unsigned short variant, unsigned char xPosition, unsigned char yPosition, unsigned char xSize, unsigned char ySize) : variant(variant), xPosition(xPosition), yPosition(yPosition), xSize(xSize), ySize(ySize) {}
};

struct MetaTexture {
    AssetStringID stringID;
    std::vector<TextureAtlasData> atlasData;

    bool operator==(const MetaTexture& other) const {
        return stringID == other.stringID;
    }
};


class AssetManager {
    public:
        AssetManager();
        ~AssetManager();

        void RegisterTextureAtlas(AssetStringID atlasStringID, Texture textureAtlas);
        void RegisterTexture(MetaTexture texture);
        void RegisterShaderProgram(AssetStringID shaderProgramStringID, Shader shaderProgram);

        Texture* GetTextureAtlas(AssetStringID atlasStringID);

        AssetStringID* GetStringID(unsigned int numericalID);
        unsigned int GetNumericalID(AssetStringID stringID);

        Shader* GetShaderProgram(AssetStringID shaderProgramStringID);

        std::vector<TextureAtlasData>* GetTextureAtlasData(unsigned int numericalID);
        std::vector<TextureAtlasData>* GetTextureAtlasData(AssetStringID stringID);

    private:
        robin_hood::unordered_flat_map<AssetStringID, Texture> textureAtlases;

        robin_hood::unordered_flat_map<unsigned int, AssetStringID> numericalIDsToStringIDs;
        robin_hood::unordered_flat_map<AssetStringID, unsigned int> stringIDsToNumericalIDs;

        robin_hood::unordered_flat_map<unsigned int, std::vector<TextureAtlasData>> atlasData;

        robin_hood::unordered_flat_map<AssetStringID, Shader> shaderPrograms;

        int latestTextureID = 0;
};

inline AssetManager assetManager = AssetManager();