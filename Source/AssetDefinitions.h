#pragma once

#include "glad/glad.h"


struct AssetStringID {
    std::string modName = "";
    std::string assetName = "";

    std::string CanonicalName() const {
        return modName + ":" + assetName;
    }

    bool operator==(const AssetStringID& other) const {
        return modName == other.modName && assetName == other.assetName;
    }

    bool operator<(const AssetStringID& other) const {
        if (modName == other.modName) {
            return assetName < other.assetName;
        }
        return modName < other.modName;
    }

    AssetStringID() : modName("kiwicubed"), assetName("air") {}
    AssetStringID(std::string modName, std::string assetName) : modName(modName), assetName(assetName) {}
};

template <>
struct std::hash<AssetStringID>{
    std::size_t operator()(const AssetStringID& stringID) const {
        return std::hash<std::string>()(stringID.CanonicalName());
    }
};

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

struct Model {
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
};

struct MetaEntityModel {
    AssetStringID stringID;
    Model model;

    bool operator==(const MetaEntityModel& other) const {
        return stringID == other.stringID;
    }
};

struct EntityType {
	AssetStringID entityStringID;
	std::unordered_map<std::string, std::string> eventsToCallbacks;

	MetaEntityModel metaModel;
	MetaTexture metaTexture;
};