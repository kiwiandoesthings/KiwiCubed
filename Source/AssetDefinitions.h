#pragma once

#include "glad/glad.h"

#include <string>


struct AssetStringID {
    char modName[64] = "kiwicubed";
    char assetName[96] = "invalid";

    std::string CanonicalName() const {
        return std::string(modName) + ":" + std::string(assetName);
    }

	std::string ModName() const {
		return std::string(modName);
	}

	std::string AssetName() const {
		return std::string(assetName);
	}

    bool operator==(const AssetStringID& other) const {
        return std::strcmp(modName, other.modName) == 0 && std::strcmp(assetName, other.assetName) == 0;
    }
    bool operator<(const AssetStringID& other) const {
		int modComparison = std::strcmp(modName, other.modName);
        if (modComparison == 0) {
            return std::strcmp(assetName, other.assetName) < 0;
        }
        return modComparison < 0;
    }

    AssetStringID() : modName("kiwicubed"), assetName("air") {}
	AssetStringID(const std::string& modName, const std::string& assetName) {
		std::strncpy(this->modName, modName.c_str(), sizeof(this->modName) - 1);
		std::strncpy(this->assetName, assetName.c_str(), sizeof(this->assetName) - 1);
	}
    AssetStringID(const char* modName, const char* assetName) {
		std::strncpy(this->modName, modName, sizeof(this->modName) - 1);
		std::strncpy(this->assetName, assetName, sizeof(this->assetName) - 1);
	}
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

	EntityType(AssetStringID typeStringID) : entityStringID(typeStringID) {}
	EntityType(AssetStringID typeStringID, std::unordered_map<std::string, std::string> eventsToCallbacks, MetaEntityModel metaModel, MetaTexture metaTexture) : entityStringID(typeStringID), eventsToCallbacks(eventsToCallbacks), metaModel(metaModel), metaTexture(metaTexture) {}
};