#include "ModHandler.h"
#include <chrono>
#include "AssetManager.h"
#include "Block.h"


ModHandler::ModHandler() {}


bool ModHandler::SetupTextureAtlasData() {
    OVERRIDE_LOG_NAME("Mod Loading");
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

    std::unordered_map<AssetStringID, std::vector<TextureAtlasData>> textureAtlasDataMap;
    std::unordered_map<AssetStringID, std::vector<AssetStringID>> blockTextureMap;

    for (const auto& entry : std::filesystem::directory_iterator("Mods")) {
        if (entry.is_directory()) {
            std::string modFolder = entry.path().string();
            
            std::ifstream file(modFolder + "/mod.json");

            if (!file.is_open()) {
                WARN("Could not find or open mod.json for folder in mods directory");
                return false;
            }

            json jsonData;
            file >> jsonData;

            std::string name = jsonData["mod_title"];
            std::string version = jsonData["mod_version"];
            std::string authors = jsonData["mod_authors"];
            std::string builtForVersion = jsonData["built_for_game_version"];

            INFO("Found mod \"" + name + "\", version: " + version + ", by: " + authors + ", built for game version " + builtForVersion);

            std::string modResources = modFolder + "/Resources/Textures";
            
            try {
                if (!std::filesystem::exists(modResources) && std::filesystem::is_directory(modResources)) {
                    return false;
                }
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
                                std::string textureID = "";

                                if (splitPosition != std::string::npos) {
                                    modID = id.substr(0, splitPosition);
                                    textureID = id.substr(splitPosition + 1);
                                } else {
                                    WARN("Tried to register texture with invalid textureID \"" + id + "\" in file: " + filePath.generic_string() + "\", skipping texture");
                                    continue;
                                }

                                textureAtlasDataMap[AssetStringID{
                                    modID.c_str(), 
                                    textureID.c_str()
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
            } catch (const std::filesystem::filesystem_error& error) {
                ERR("Filesystem error: " + std::string(error.what()) + ", aborting");
                return false;
            }

            std::string blocks = modFolder + "/Resources/Blocks";
            
            try {
                if (!std::filesystem::exists(blocks) && std::filesystem::is_directory(blocks)) {
                    return false;
                }
                for (const auto& entry : std::filesystem::directory_iterator(blocks)) {
                    if (std::filesystem::is_regular_file(entry.status())) {
                        auto filePath = entry.path();
                        if (filePath.filename() == "blocks.json") {
                            std::ifstream file(filePath);

                            json jsonData;

                            file >> jsonData;

                            for (const auto& block : jsonData["blocks"]) {
                                std::string id = block["blockID"];
                                std::string front = block["texture_front"];
                                std::string back = block["texture_back"];
                                std::string left = block["texture_left"];
                                std::string right = block["texture_right"];
                                std::string top = block["texture_top"];
                                std::string bottom = block["texture_bottom"];

                                std::vector<std::string> basicIDs;
                                basicIDs.reserve(6);
                                basicIDs.emplace_back(front);
                                basicIDs.emplace_back(back);
                                basicIDs.emplace_back(left);
                                basicIDs.emplace_back(right);
                                basicIDs.emplace_back(top);
                                basicIDs.emplace_back(bottom);

                                std::vector<AssetStringID> textureAssetIDs;
                                textureAssetIDs.reserve(6);

                                for (const auto& id : basicIDs) {
                                    size_t splitPosition = id.find(":");

                                    std::string modID = "";
                                    std::string textureID = "";

                                    if (splitPosition != std::string::npos) {
                                        modID = id.substr(0, splitPosition);
                                        textureID = id.substr(splitPosition + 1);
                                    } else {
                                        WARN("Tried to register texture with invalid textureID \"" + id + "\" in file: " + filePath.generic_string() + "\", skipping texture");
                                        continue;
                                    }

                                    textureAssetIDs.emplace_back(AssetStringID{modID, textureID});
                                }

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

                                blockTextureMap[AssetStringID{
                                    modID.c_str(), 
                                    blockID.c_str()
                                }] = (textureAssetIDs);
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
            ERR("Tried to register texture with higher variant number than actual variants defined for block \"" + blockTypeData.first.CanonicalName() + ", aborting");
            return false;
        }

        assetManager.RegisterTexture(MetaTexture{{blockTypeData.first}, blockTypeData.second});
    }

    for (const auto& block : blockTextureMap) {
        std::vector<AssetStringID> textureIDs = block.second;

        std::vector<unsigned char> faceTextureIDs;
        std::vector<MetaTexture> textureAtlasDatas;
        
        for (const auto& stringID : block.second) {
            auto iterator = textureAtlasDataMap.find(stringID);
            if (iterator == textureAtlasDataMap.end()) {
                ERR("Tried to get string ID for texture with string ID \"" + stringID.CanonicalName() + "\" that did not exist, aborting");
            }
            textureAtlasDatas.push_back(MetaTexture{iterator->first, iterator->second});

            for (MetaTexture atlasDefinition : textureAtlasDatas) {
                if (iterator->first == atlasDefinition.stringID) {
                    faceTextureIDs.push_back(std::distance(textureAtlasDatas.begin(), std::find(textureAtlasDatas.begin(), textureAtlasDatas.end(), atlasDefinition)));
                    break;
                }
            }
        }

        BlockManager::GetInstance().RegisterBlockType(block.first, BlockType{block.first, textureAtlasDatas, faceTextureIDs});
    }

    INFO("Loading mods took " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startTime).count()) + "us");

    return true;
}

ModHandler::~ModHandler() {}