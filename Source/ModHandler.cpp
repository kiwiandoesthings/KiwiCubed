#include "ModHandler.h"


ModHandler::ModHandler() {}


bool ModHandler::SetupTextureAtlasData() {
    OVERRIDE_LOG_NAME("Mod Loading");

    std::unordered_map<TextureStringID, std::vector<TextureAtlasData>> textureAtlasDataMap;

    for (const auto& entry : std::filesystem::directory_iterator("Mods")) {
        if (entry.is_directory()) {
            std::string modFolder = entry.path().string();
            std::string modResources = modFolder + "/Resources/Textures";
            
            try {
                if (std::filesystem::exists(modResources) && std::filesystem::is_directory(modResources)) {
                    for (const auto& entry : std::filesystem::directory_iterator(modResources)) {
                        if (std::filesystem::is_regular_file(entry.status())) {
                            auto filePath = entry.path();
                            if (filePath.extension() == ".json") {
                                std::ifstream file(filePath);
                                json jsonData;

                                file >> jsonData;

                                for (const auto& texture : jsonData["textures"]) {
                                    std::string id = texture["id"];
                                    unsigned short variant = texture["variant"];
                                    unsigned char xPosition = texture["xPosition"];
                                    unsigned char yPosition = texture["yPosition"];
                                    unsigned char xSize = texture["xSize"];
                                    unsigned char ySize = texture["ySize"];

                                    size_t splitPosition = id.find(":");
                                    std::string modID = "";
                                    std::string blockID = "";
                                    if (splitPosition != std::string::npos) {
                                        modID = id.substr(0, splitPosition);
                                        blockID = id.substr(splitPosition + 1);
                                    } else {
                                        WARN("Tried to register texture with invalid textureID \"" + id + "\" in file: " + filePath.generic_string() + "\", skipping texture");
                                        continue;
                                    }

                                    textureAtlasDataMap[TextureStringID{
                                        modID.c_str(), 
                                        blockID.c_str()
                                    }].push_back(TextureAtlasData{
                                        variant,
                                        xPosition,
                                        yPosition,
                                        xSize,
                                        ySize
                                    });
                                }
                            }
                        }
                    }
                }
            } catch (const std::filesystem::filesystem_error& error) {
                ERR("Filesystem error: " + std::string(error.what()) + ", aborting");
                return false;
            }
        }
    }

    for (const auto& blockTypeData : textureAtlasDataMap) {
        unsigned short highestVariant = 0;
        for (const auto& texture : blockTypeData.second) {
            if (texture.variant > highestVariant) {
                highestVariant = texture.variant;
            }
        }

        if (highestVariant > blockTypeData.second.size() - 1) {
            ERR("Tried to register texture with higher variant count than actual variants defined for block \"" + blockTypeData.first.CanonicalName() + ", aborting");
            return false;
        }

        textureManager.RegisterTexture(MetaTexture{
            blockTypeData.first,
            blockTypeData.second
        });
    } 

    return true;
}

ModHandler::~ModHandler() {}