#include "ModHandler.h"
#include <chrono>
#include "AssetDefinitions.h"
#include "AssetManager.h"
#include "Block.h"
#include "EntityManager.h"


ModHandler::ModHandler() {}


bool ModHandler::SetupTextureAtlasData() {
    OVERRIDE_LOG_NAME("Mod Loading");
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

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
            std::string modNamespace = jsonData["namespace"];

            if (builtForVersion != Globals::GetInstance().gameVersion) {
                WARN("Mod below is built for different game version than currently running!");
            }

            for (int iterator = 0; iterator < modNamespaces.size(); iterator++) {
                if (modNamespaces[iterator] == modNamespace) {
                    CRITICAL("Tried to register multiple mods with the same namespace, \"" + modNamespace + "\"");
                    psnip_trap();
                }
            }

            modNamespaces.push_back(modNamespace);

            INFO("Found mod \"" + name + "\", version {" + version + "}, by \"" + authors + "\", built for game version {" + builtForVersion + "}, using namespace \"" + modNamespace + "\"");

            std::string modTextures = modFolder + "/Resources/Textures";
            
            try {
                if (!std::filesystem::exists(modTextures) || !std::filesystem::is_directory(modTextures)) {
                    ERR("Could not find mod's textures folder");
                    return false;
                }
                for (const auto& entry : std::filesystem::directory_iterator(modTextures)) {
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
                                std::string assetID = "";

                                if (splitPosition != std::string::npos) {
                                    modID = id.substr(0, splitPosition);
                                    assetID = "texture/" + id.substr(splitPosition + 1);
                                } else {
                                    WARN("Tried to register texture with invalid string id \"" + id + "\" in file: " + filePath.generic_string() + "\", skipping");
                                    continue;
                                }

                                textureAtlasDataMap[AssetStringID{
                                    modID, 
                                    assetID
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
                if (!std::filesystem::exists(blocks) || !std::filesystem::is_directory(blocks)) {
                    ERR("Could not find mod's blocks folder");
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
                                        textureID = "texture/" + id.substr(splitPosition + 1);
                                    } else {
                                        WARN("Tried to register texture with invalid string id \"" + id + "\" in file: " + filePath.generic_string() + "\", skipping");
                                        continue;
                                    }

                                    textureAssetIDs.emplace_back(AssetStringID{modID, textureID});
                                }

                                size_t splitPosition = id.find(":");

                                std::string modID = "";
                                std::string assetID = "";

                                if (splitPosition != std::string::npos) {
                                    modID = id.substr(0, splitPosition);
                                    assetID = "block/" + id.substr(splitPosition + 1);
                                } else {
                                    WARN("Tried to register block with invalid string id \"" + id + "\" in file: " + filePath.generic_string() + "\", skipping");
                                    continue;
                                }

                                blockTextureMap[AssetStringID{
                                    modID, 
                                    assetID
                                }] = (textureAssetIDs);
                            }
                        }
                    }
                }
            } catch (const std::filesystem::filesystem_error& error) {
                ERR("Filesystem error: " + std::string(error.what()) + ", aborting");
                return false;
            }

            std::string modModels = modFolder + "/Resources/Models";
            
            try {
                if (!std::filesystem::exists(modModels) || !std::filesystem::is_directory(modModels)) {
                    ERR("Could not find mod's models folder");
                    return false;
                }
                for (const auto& entry : std::filesystem::directory_iterator(modModels)) {
                    if (std::filesystem::is_regular_file(entry.status())) {
                        auto filePath = entry.path();
                        if (filePath.extension() == ".json") {
                            std::ifstream file(filePath);

                            json jsonData;

                            file >> jsonData;

                            std::string id = jsonData["id"];

                            size_t splitPosition = id.find(":");

                            std::string modID = "";
                            std::string assetID = "";

                            if (splitPosition != std::string::npos) {
                                modID = id.substr(0, splitPosition);
                                assetID = "model/" + id.substr(splitPosition + 1);
                            } else {
                                WARN("Tried to register entity model with invalid string id \"" + id + "\" in file: " + filePath.generic_string() + "\", skipping");
                                continue;
                            }

                            AssetStringID modelStringID = AssetStringID{modID, assetID};

                            std::vector<GLfloat> modelVertices;
                            std::vector<GLuint> modelIndices;

                            for (const auto& vertex : jsonData["model"]["vertices"]) {
                                for (const auto& coordinate : vertex) {
                                    modelVertices.push_back(coordinate);
                                }
                            }

                            for (const auto& index : jsonData["model"]["indices"]) {
                                modelIndices.push_back(index);
                            }

                            MetaEntityModel entityModel = MetaEntityModel{
                                modelStringID,
                                Model{
                                    modelVertices,
                                    modelIndices
                                }
                            };

                            assetManager.RegisterEntityModel(entityModel);

                            EntityManager::GetInstance().RegisterEntity(EntityType{
                                AssetStringID{"kiwicubed", "entity/" + id.substr(splitPosition + 1)},
                                entityModel,
                                MetaTexture{}
                            });
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

        BlockManager::GetInstance().RegisterBlockType(BlockType{block.first, textureAtlasDatas, faceTextureIDs});
    }

    INFO("Loading mods took " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count()) + "us");

    return true;
}

ModHandler::~ModHandler() {}