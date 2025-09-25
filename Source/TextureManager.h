#pragma once

#include <klogger.hpp>
#include <debug-trap.h>

#include <string>
#include <unordered_map>


struct TextureStringID {
    std::string modName = "";
    std::string textureName = "";

    std::string CanonicalName() const {
        return modName + ":" + textureName;
    }

    bool operator==(const TextureStringID& other) const {
        return modName == other.modName && textureName == other.textureName;
    }
};

struct TextureAtlasData {
    const unsigned short variant = 0;
    const unsigned char xPosition = 0;
    const unsigned char yPosition = 0;
    const unsigned char xSize = 0;
    const unsigned char ySize = 0;
};

struct MetaTexture {
    TextureStringID stringID;
    std::vector<TextureAtlasData> atlasData;
};

template <>
struct std::hash<TextureStringID>{
    std::size_t operator()(const TextureStringID& stringID) const {
        return std::hash<std::string>()(stringID.modName + ":" + stringID.textureName);
    }
};

class TextureManager {
    public:
        TextureManager();
        ~TextureManager();

        void RegisterTexture(MetaTexture texture);

        TextureStringID* GetStringID(unsigned int numericalID);
        unsigned int GetNumericalID(TextureStringID stringID);

        std::vector<TextureAtlasData>* GetTextureAtlasData(unsigned int numericalID);
        std::vector<TextureAtlasData>* GetTextureAtlasData(TextureStringID stringID);

    private:
        std::unordered_map<unsigned int, TextureStringID> numericalIDsToStringIDs;
        std::unordered_map<TextureStringID, unsigned int> stringIDsToNumericalIDs;

        std::unordered_map<unsigned int, std::vector<TextureAtlasData>> atlasData;

        int latestTextureID = 0;
};

inline TextureManager textureManager = TextureManager();