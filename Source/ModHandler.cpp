#include "ModHandler.h"
#include <chrono>
#include "AssetDefinitions.h"
#include "AssetManager.h"
#include "Block.h"
#include "EntityManager.h"
#include "Events.h"
#include "GenerateAngelScriptLanguageData.cpp"



ModHandler& ModHandler::GetInstance() {
    static ModHandler instance;
    return instance;
}


bool ModHandler::LoadModData() {
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

            OrderedJSON modJSON;
            file >> modJSON;

            std::string name = modJSON["mod_title"];
            std::string version = modJSON["mod_version"];
            std::string authors = modJSON["mod_authors"];
            std::string builtForVersion = modJSON["built_for_game_version"];
            std::string modNamespace = modJSON["namespace"];

            if (builtForVersion != Globals::GetInstance().projectVersion) {
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

                            OrderedJSON configJSON;

                            file >> configJSON;

                            for (const auto& texture : configJSON["textures"]) {
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
                                    WARN("Tried to register texture with invalid string ID \"" + id + "\" in file: " + filePath.generic_string() + "\", skipping");
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

                            OrderedJSON configJSON;

                            file >> configJSON;

                            for (const auto& block : configJSON["blocks"]) {
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
                                        WARN("Tried to register texture with invalid string ID \"" + id + "\" in file: " + filePath.generic_string() + "\", skipping");
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
                                    WARN("Tried to register block with invalid string ID \"" + id + "\" in file: " + filePath.generic_string() + "\", skipping");
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

            for (const auto& block : blockTextureMap) {
                std::vector<AssetStringID> textureIDs = block.second;
        
                std::vector<unsigned char> faceTextureIDs;
                std::vector<MetaTexture> textureAtlasDatas;

                for (const auto& stringID : block.second) {
                    auto iterator = textureAtlasDataMap.find(stringID);
                    if (iterator == textureAtlasDataMap.end()) {
                        ERR("Tried to get string ID for texture with string ID \"" + stringID.CanonicalName() + "\" that did not exist, aborting");
                        return false;
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

                            OrderedJSON configJSON;

                            file >> configJSON;

                            std::string id = configJSON["id"];

                            size_t splitPosition = id.find(":");

                            std::string modID = "";
                            std::string assetID = "";

                            if (splitPosition != std::string::npos) {
                                modID = id.substr(0, splitPosition);
                                assetID = "model/" + id.substr(splitPosition + 1);
                            } else {
                                WARN("Tried to register entity model with invalid string ID \"" + id + "\" in file: " + filePath.generic_string() + "\", skipping");
                                continue;
                            }

                            AssetStringID modelStringID = AssetStringID{modID, assetID};

                            std::vector<GLfloat> modelVertices;
                            std::vector<GLuint> modelIndices;

                            for (const auto& vertex : configJSON["model"]["vertices"]) {
                                for (const auto& coordinate : vertex) {
                                    modelVertices.push_back(coordinate);
                                }
                            }

                            for (const auto& index : configJSON["model"]["indices"]) {
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
                        }
                    }
                }
            } catch (const std::filesystem::filesystem_error& error) {
                ERR("Filesystem error: " + std::string(error.what()) + ", aborting");
                return false;
            }

            std::string modEntities = modFolder + "/Resources/Entities";
            
            try {
                if (!std::filesystem::exists(modModels) || !std::filesystem::is_directory(modEntities)) {
                    ERR("Could not find mod's entities folder");
                    return false;
                }
                for (const auto& entry : std::filesystem::directory_iterator(modEntities)) {
                    if (std::filesystem::is_regular_file(entry.status())) {
                        auto filePath = entry.path();
                        if (filePath.extension() == ".json") {
                            std::ifstream file(filePath);

                            OrderedJSON configJSON;

                            file >> configJSON;

                            std::string id = configJSON["id"];

                            size_t splitPosition = id.find(":");

                            std::string modID = "";
                            std::string assetID = "";

                            if (splitPosition != std::string::npos) {
                                modID = id.substr(0, splitPosition);
                                assetID = "entity/" + id.substr(splitPosition + 1);
                            } else {
                                WARN("Tried to register entity with invalid string ID \"" + id + "\" in file: " + filePath.generic_string() + "\", skipping");
                                continue;
                            }

                            AssetStringID entityStringID = AssetStringID{modID, assetID};

                            std::string entityTextureID = configJSON["texture"];
                            std::string entityModelID = configJSON["model"];

                            AssetStringID textureStructID = assetManager.StringToAssetStruct(entityTextureID, "texture");
                            AssetStringID modelStructID = assetManager.StringToAssetStruct(entityModelID, "model");

                            std::vector<TextureAtlasData>* entityTextureData = assetManager.GetTextureAtlasData(textureStructID);
                            Model* entityModel = assetManager.GetEntityModel(modelStructID);

                            std::unordered_map<std::string, std::string> eventsToCallbacks;
                            std::vector<std::string> callbacks;

                            for (const auto& [event, callback] : configJSON["eventCallbacks"].items()) {
                                eventsToCallbacks.insert({event, callback});
                                callbacks.push_back(callback);
                            }

                            entityManager.RegisterEntityType(EntityType{
                                entityStringID,
                                eventsToCallbacks,
                                MetaEntityModel{modelStructID, *entityModel},
                                MetaTexture{textureStructID, *entityTextureData}
                            });

                            modToEntityCallbacks.insert({modNamespace, callbacks});
                        }
                    }
                }
            } catch (const std::filesystem::filesystem_error& error) {
                ERR("Filesystem error: " + std::string(error.what()) + ", aborting");
                return false;
            }

            std::string modScripts = modFolder + "/Scripts";
            
            try {
                if (!std::filesystem::exists(modScripts) || !std::filesystem::is_directory(modScripts)) {
                    ERR("Could not find mod's scripts folder");
                    return false;
                }
                std::string scriptPath;
                for (const auto& entry : std::filesystem::directory_iterator(modScripts)) {
                    if (std::filesystem::is_regular_file(entry.status())) {
                        auto filePath = entry.path();
                        if (filePath.extension() == ".as") {
                            scriptPath = filePath.generic_string();
                            modNamespacesToScripts.insert(std::make_pair(modNamespace, scriptPath));
                        }
                    }
                }
            } catch (const std::filesystem::filesystem_error& error) {
                ERR("Filesystem error: " + std::string(error.what()) + ", aborting");
                return false;
            }
        }
    }

    INFO("Loading mods took " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count()) + "us");

    return true;
}

bool ModHandler::LoadModScripts() {
    OVERRIDE_LOG_NAME("Mod Scripting Environment Initialization");
	engine = asCreateScriptEngine();
	RegisterStdString(engine);
	RegisterScriptArray(engine, true);
	RegisterScriptDictionary(engine);
	RegisterScriptMath(engine);
	int result = engine->SetMessageCallback(asFUNCTION(ModError), &std::cout, asCALL_CDECL);
	if (result < 0) {
		ERR("Failed to set message callback");
		return false;
	}

	// Types
	//  AssetStringID
	engine->RegisterObjectType("AssetStringID", sizeof(AssetStringID), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK);
	engine->RegisterObjectBehaviour("AssetStringID", asBEHAVE_CONSTRUCT, "void f(const string &in, const string &in)", asFUNCTION(+[](const std::string& modName, const std::string& assetName, void* memory) {
		new(memory) AssetStringID(modName, assetName);
	}), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("AssetStringID", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(+[](void* memory){
		reinterpret_cast<AssetStringID*>(memory)->~AssetStringID();
	}), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("AssetStringID", "string CanonicalName() const", asMETHOD(AssetStringID, CanonicalName), asCALL_THISCALL);
	engine->RegisterObjectProperty("AssetStringID", "string modName", offsetof(AssetStringID, modName));
	engine->RegisterObjectProperty("AssetStringID", "string assetName", offsetof(AssetStringID, assetName));
	//  EventType
	engine->RegisterEnum("EventType");
	engine->RegisterEnumValue("EventType", "EVENT_META_WINDOW_MINIMIZE", EventType::EVENT_META_WINDOW_MINIMIZE);
	engine->RegisterEnumValue("EventType", "EVENT_META_WINDOW_MAXIMIZE", EventType::EVENT_META_WINDOW_MAXIMIZE);
	engine->RegisterEnumValue("EventType", "EVENT_META_WINDOW_RESIZE", EventType::EVENT_META_WINDOW_RESIZE);
	engine->RegisterEnumValue("EventType", "EVENT_WORLD_TICK", EventType::EVENT_WORLD_TICK);
	engine->RegisterEnumValue("EventType", "EVENT_WORLD_PLAYER_BLOCK", EventType::EVENT_WORLD_PLAYER_BLOCK);
	engine->RegisterEnumValue("EventType", "EVENT_WORLD_PLAYER_MOVE", EventType::EVENT_WORLD_PLAYER_MOVE);
	engine->RegisterEnumValue("EventType", "EVENT_WORLD_ENTITY_BLOCK", EventType::EVENT_WORLD_ENTITY_BLOCK);
	engine->RegisterEnumValue("EventType", "EVENT_WORLD_ENTITY_HURT", EventType::EVENT_WORLD_ENTITY_HURT);
	engine->RegisterEnumValue("EventType", "EVENT_WORLD_ENTITY_ATTACK", EventType::EVENT_WORLD_ENTITY_ATTACK);
	engine->RegisterEnumValue("EventType", "EVENT_WORLD_GENERIC_BLOCK", EventType::EVENT_WORLD_GENERIC_BLOCK);
	//  BlockEventType
	engine->RegisterEnum("BlockEventType");
	engine->RegisterEnumValue("BlockEventType", "BLOCK_MINED", BlockEventType::BLOCK_MINED);
	engine->RegisterEnumValue("BlockEventType", "BLOCK_PLACED", BlockEventType::BLOCK_PLACED);
	engine->RegisterEnumValue("BlockEventType", "BLOCK_REPLACED", BlockEventType::BLOCK_REPLACED);
	engine->RegisterEnumValue("BlockEventType", "BLOCK_INTERACTED", BlockEventType::BLOCK_INTERACTED);
	engine->RegisterEnumValue("BlockEventType", "BLOCK_TICK_UPDATE", BlockEventType::BLOCK_TICK_UPDATE);
	engine->RegisterEnumValue("BlockEventType", "BLOCK_NEIGHBOR_UPDATE", BlockEventType::BLOCK_NEIGHBOR_UPDATE);
	//  EventWorldTick
	engine->RegisterObjectType("EventWorldTick", sizeof(EventWorldTick), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<EventWorldTick>());
	engine->RegisterObjectProperty("EventWorldTick", "uint64 tickNumber", offsetof(EventWorldTick, tickNumber));
	//  EventWorldPlayerBlock
	engine->RegisterObjectType("EventWorldPlayerBlock", sizeof(EventWorldPlayerBlock), asOBJ_VALUE | asGetTypeTraits<EventWorldPlayerBlock>());
	engine->RegisterObjectBehaviour("EventWorldPlayerBlock", asBEHAVE_CONSTRUCT, "void f(const BlockEventType &in, const uint64 &in, const int &in, const int &in, const int &in, const int &in, const int &in, const AssetStringID &in, const AssetStringID &in)", asFUNCTION(+[](const BlockEventType& blockEventType, const unsigned long long& playerAUID, const int& chunkX, const int& chunkY, const int& chunkZ, const int& blockX, const int& blockY, const int& blockZ, const AssetStringID& oldBlockStringID, const AssetStringID& newBlockStringID, void* memory){
		new(memory) EventWorldPlayerBlock(blockEventType, playerAUID, chunkX, chunkY, chunkZ, blockX, blockY, blockZ, oldBlockStringID, newBlockStringID);
	}), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("EventWorldPlayerBlock", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(+[](void* memory) {
		reinterpret_cast<EventWorldPlayerBlock*>(memory)->~EventWorldPlayerBlock();
	}), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectProperty("EventWorldPlayerBlock", "BlockEventType blockEventType", offsetof(EventWorldPlayerBlock, blockEventType));
	engine->RegisterObjectProperty("EventWorldPlayerBlock", "uint64 playerAUID", offsetof(EventWorldPlayerBlock, playerAUID));
	engine->RegisterObjectProperty("EventWorldPlayerBlock", "int chunkX", offsetof(EventWorldPlayerBlock, chunkX));
	engine->RegisterObjectProperty("EventWorldPlayerBlock", "int chunkY", offsetof(EventWorldPlayerBlock, chunkY));
	engine->RegisterObjectProperty("EventWorldPlayerBlock", "int chunkZ", offsetof(EventWorldPlayerBlock, chunkZ));
	engine->RegisterObjectProperty("EventWorldPlayerBlock", "int blockX", offsetof(EventWorldPlayerBlock, blockX));
	engine->RegisterObjectProperty("EventWorldPlayerBlock", "int blockY", offsetof(EventWorldPlayerBlock, blockY));
	engine->RegisterObjectProperty("EventWorldPlayerBlock", "int blockZ", offsetof(EventWorldPlayerBlock, blockZ));
	engine->RegisterObjectProperty("EventWorldPlayerBlock", "AssetStringID oldBlockStringID", offsetof(EventWorldPlayerBlock, oldBlockStringID));
	engine->RegisterObjectProperty("EventWorldPlayerBlock", "AssetStringID newBlockStringID", offsetof(EventWorldPlayerBlock, newBlockStringID));
	//  Vec3
	engine->RegisterObjectType("Vec3", sizeof(Vec3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Vec3>());
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(+[](void* memory) {
		new(memory) Vec3(0.0f, 0.0f, 0.0f);
	}), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT, "void f(const float &in, const float &in, const float &in)", asFUNCTION(+[](const float& x, const float& y, const float& z, void* memory) {
		new(memory) Vec3(x, y, z);
	}), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT, "void f(const int &in, const int &in, const int &in)", asFUNCTION(+[](const int& x, const int& y, const int& z, void* memory) {
		new(memory) Vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
	}), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(+[](void* memory) {
		reinterpret_cast<Vec3*>(memory)->~Vec3();
	}), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Vec3", "Vec3 opAdd(const Vec3 &in) const", asMETHODPR(Vec3, operator+, (Vec3&), Vec3), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vec3", "Vec3 opSub(const Vec3 &in) const", asMETHODPR(Vec3, operator-, (Vec3&), Vec3), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vec3", "Vec3 opMul(const Vec3 &in) const", asMETHODPR(Vec3, operator*, (Vec3&), Vec3), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vec3", "Vec3 opDiv(const Vec3 &in) const", asMETHODPR(Vec3, operator/, (Vec3&), Vec3), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vec3", "bool opEquals(const Vec3 &in) const", asMETHODPR(Vec3, operator==, (Vec3&), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vec3", "Vec3 &opAssign(const Vec3 &in)", asMETHODPR(Vec3, operator=, (Vec3&), Vec3&), asCALL_THISCALL);
	engine->RegisterObjectProperty("Vec3", "float x", offsetof(Vec3, x));
	engine->RegisterObjectProperty("Vec3", "float y", offsetof(Vec3, y));
	engine->RegisterObjectProperty("Vec3", "float z", offsetof(Vec3, z));
	//  Chunk
	engine->RegisterObjectType("Chunk", sizeof(Chunk), asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("Chunk", "int chunkX", offsetof(Chunk, chunkX));
	engine->RegisterObjectProperty("Chunk", "int chunkY", offsetof(Chunk, chunkY));
	engine->RegisterObjectProperty("Chunk", "int chunkZ", offsetof(Chunk, chunkZ));
	engine->RegisterObjectMethod("Chunk", "uint8 GetGenerationStatus()", asMETHOD(Chunk, GetGenerationStatus), asCALL_THISCALL);
	// EntityData
	engine->RegisterObjectType("EntityData", sizeof(EntityData), asOBJ_VALUE | asGetTypeTraits<EntityData>());
	engine->RegisterObjectBehaviour("EntityData", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(+[](void* memory) {
		new(memory) EntityData();
	}), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("EntityData", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(+[](void* memory) {
		reinterpret_cast<EntityData*>(memory)->~EntityData();
	}), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectProperty("EntityData", "Chunk@ currentChunkPtr", offsetof(EntityData, currentChunkPtr));
	engine->RegisterObjectProperty("EntityData", "float terminalVelocity", offsetof(EntityData, terminalVelocity));
	engine->RegisterObjectProperty("EntityData", "string name", offsetof(EntityData, name));
	engine->RegisterObjectProperty("EntityData", "float moveSpeed", offsetof(EntityData, moveSpeed));
	engine->RegisterObjectProperty("EntityData", "float jumpHeight", offsetof(EntityData, jumpHeight));
	engine->RegisterObjectProperty("EntityData", "float walkModifier", offsetof(EntityData, walkModifier));
	engine->RegisterObjectProperty("EntityData", "float jumpModifier", offsetof(EntityData, jumpModifier));
	engine->RegisterObjectProperty("EntityData", "bool applyGravity", offsetof(EntityData, applyGravity));
	engine->RegisterObjectProperty("EntityData", "bool applyCollision", offsetof(EntityData, applyCollision));
	engine->RegisterObjectProperty("EntityData", "bool isGrounded", offsetof(EntityData, isGrounded));
	engine->RegisterObjectProperty("EntityData", "bool isJumping", offsetof(EntityData, isJumping));
	engine->RegisterObjectProperty("EntityData", "bool isPlayer", offsetof(EntityData, isPlayer));
	//  EntityTransform
	engine->RegisterObjectType("EntityTransform", sizeof(EntityTransform), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<EntityTransform>());
	engine->RegisterObjectMethod("EntityTransform", "Vec3 GetPosition() const", asMETHOD(EntityTransform, GetPosition), asCALL_THISCALL);
	engine->RegisterObjectMethod("EntityTransform", "void SetPosition(const Vec3 &in)", asMETHOD(EntityTransform, SetPosition), asCALL_THISCALL);
	engine->RegisterObjectMethod("EntityTransform", "Vec3 GetOrientation() const", asMETHOD(EntityTransform, GetOrientation), asCALL_THISCALL);
	engine->RegisterObjectMethod("EntityTransform", "void SetOrientation(const Vec3 &in)", asMETHOD(EntityTransform, SetOrientation), asCALL_THISCALL);
	engine->RegisterObjectMethod("EntityTransform", "Vec3 GetUpDirection() const", asMETHOD(EntityTransform, GetUpDirection), asCALL_THISCALL);
	engine->RegisterObjectMethod("EntityTransform", "void SetUpDirection(const Vec3 &in)", asMETHOD(EntityTransform, SetUpDirection), asCALL_THISCALL);
	engine->RegisterObjectMethod("EntityTransform", "Vec3 GetVelocity() const", asMETHOD(EntityTransform, GetVelocity), asCALL_THISCALL);
	engine->RegisterObjectMethod("EntityTransform", "void SetVelocity(const Vec3 &in)", asMETHOD(EntityTransform, SetVelocity), asCALL_THISCALL);
	engine->RegisterObjectMethod("EntityTransform", "Vec3 GetGlobalChunkPosition() const", asMETHOD(EntityTransform, GetGlobalChunkPosition), asCALL_THISCALL);
	engine->RegisterObjectMethod("EntityTransform", "Vec3 GetLocalChunkPosition() const", asMETHOD(EntityTransform, GetLocalChunkPosition), asCALL_THISCALL);

	// Functions
	engine->RegisterGlobalFunction("void Log(const string &in)", asFUNCTION(ModLog), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool RegisterEventToEntityType(EventType, const AssetStringID &in)", asFUNCTION(EventManager::RegisterEventToEntityType), asCALL_CDECL);
	engine->RegisterGlobalFunction("void RegisterFunctionToEvent(const EventType& in)", asFUNCTION(+[](const EventType& eventType) {
		asIScriptContext* context = asGetActiveContext();
		EventManager::GetInstance().RegisterScriptToEvent(eventType, context->GetFunction()->GetModule()->GetName());
	}), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 SpawnEntity(const AssetStringID &in, const AssetStringID &in, const AssetStringID &in, const AssetStringID &in, const float &in, const float &in, const float &in)", asFUNCTION(+[](const AssetStringID& entityType, const AssetStringID& entityModel, const AssetStringID& entityAtlas, const AssetStringID& entityTexture, const float& entityX, const float& entityY, const float& entityZ) {
		return EntityManager::GetInstance().SpawnEntity(entityType, entityModel, entityAtlas, entityTexture, glm::vec3(entityX, entityY, entityZ));
	}), asCALL_CDECL);
	engine->RegisterGlobalFunction("AssetStringID GetBlockTextureAtFace(const AssetStringID& in, const int& in)", asFUNCTION(+[](const AssetStringID& blockStringID, const int& face) {
		BlockType* blockType = BlockManager::GetInstance().GetBlockType(blockStringID);
		int faceID = 0;
		switch (face) {
			case 0:
				faceID = blockType->frontFaceID;
			case 1:
				faceID = blockType->backFaceID;
			case 2:
				faceID = blockType->leftFaceID;
			case 3:
				faceID = blockType->rightFaceID;
			case 4:
				faceID = blockType->topFaceID;
			case 5:
				faceID = blockType->bottomFaceID;
		}
		return blockType->metaTextures[faceID].stringID;
	}), asCALL_CDECL);
	engine->RegisterGlobalFunction("EntityTransform GetEntityTransform(const uint64 &in)", asFUNCTION(+[](const uint64_t& entityID) {
		return EntityManager::GetInstance().GetEntity(entityID).GetEntityTransform();
	}), asCALL_CDECL);
	engine->RegisterGlobalFunction("void SetEntityTransform(const uint64 &in, const EntityTransform &in)", asFUNCTION(+[](const uint64_t& entityID, const EntityTransform& transform) {
		EntityManager::GetInstance().GetEntity(entityID).SetEntityTransform(transform);
	}), asCALL_CDECL);

	//GenerateScriptPredefined(engine, "Github Repos/kiwicubed/Mods/kiwicubed/Scripts/as.predefined");

	OVERRIDE_LOG_NAME("Mod Script Loading");
    for (auto iterator = modNamespacesToScripts.begin(); iterator != modNamespacesToScripts.end(); iterator++) {
        std::string modNamespace = iterator->first;
        std::string modScript = iterator->second;

		asIScriptModule* module = engine->GetModule(modNamespace.c_str(), asGM_ALWAYS_CREATE);
		modNamespacesToModules.insert({modNamespace, module});
		std::string script = LoadFile(modScript);
		int scriptAddResult = module->AddScriptSection(modScript.c_str(), script.c_str(), script.size());
		if (scriptAddResult < 0) {
			ERR("Failed to add script to AngelScript module in mod with namespace \"" + modNamespace + "\"");
			return false;
		}
		int moduleBuildResult = module->Build();
		if (moduleBuildResult < 0) {
			ERR("Failed to build mod module in mod with namespace \"" + modNamespace + "\"");
			return false;
		}

		asIScriptFunction* function = module->GetFunctionByDecl("void Initialize()");
		if (!function) {
			ERR("Failed to find \"Initialize\" function in mod with namespace \"" + modNamespace + "\"");
			return false;
		}
    }

    return true;
}

bool ModHandler::RunModEntrypoints() {
	OVERRIDE_LOG_NAME("Mod Entrypoint Execution");
	for (auto iterator = modNamespacesToModules.begin(); iterator != modNamespacesToModules.end(); iterator++) {
		std::string modNamespace = iterator->first;
		asIScriptModule* module = iterator->second;

		asIScriptFunction* entrypoint = module->GetFunctionByName("Initialize");
		if (!entrypoint) {
			ERR("Failed to find \"Initialize\" function in mod with namespace \"" + modNamespace + "\", this should happen under any circumstances, please report a bug");
			return false;
		}
		asIScriptContext* context = engine->CreateContext();
		context->Prepare(entrypoint);
		int result = context->Execute();
		if (result != asEXECUTION_FINISHED) {
			ERR("Initialization in mod with namespace \"" + modNamespace + "\" failed");
			return false;
		}
		context->Release();
	}

    return true;
}

void ModHandler::CallModFunction(const std::string& moduleName, const std::string& functionName, const std::vector<void*>& arguments, const std::vector<int>& argumentTypes) {
	OVERRIDE_LOG_NAME("Mod Script Execution");
	asIScriptModule* module = modNamespacesToModules.find("kiwicubed")->second;
	// TODO: valid module checking
	asIScriptFunction* function = module->GetFunctionByName(functionName.c_str());
	if (!function || function == nullptr) {
		CRITICAL("Tried to call function \"" + functionName + "\" in module from namespace \"" + moduleName + "\" that didn't exist, aborting");
		psnip_trap();
	}

	asIScriptContext* context = engine->CreateContext();
	int prepareResult = context->Prepare(function);
	if (prepareResult != asSUCCESS) {
		CRITICAL("Failed to prepare function \"" + functionName + "\" from module \"" + moduleName + "\", with result enum of {" + std::to_string(prepareResult) + "}");
	}

	for (unsigned int iterator = 0; iterator < arguments.size(); iterator++) {
		int type = argumentTypes[iterator];
		void* argument = arguments[iterator];

		if (type == asTYPEID_APPOBJECT) {
			context->SetArgObject(iterator, argument);
		} else if (type == asTYPEID_INT32) {
			context->SetArgDWord(iterator, *reinterpret_cast<int32_t*>(argument));
		} else if (type == asTYPEID_UINT64)  {
			context->SetArgQWord(iterator, *reinterpret_cast<uint64_t*>(argument));
		} else {
			WARN("Tried to call function with unsupported argument type: {" + std::to_string(type) + "}, aborting");
			return;
		}
	}

	int executionResult = context->Execute();
	if (executionResult != asEXECUTION_FINISHED) {
		if (executionResult == asEXECUTION_EXCEPTION) {
			asIScriptFunction* exceptionFunction = context->GetExceptionFunction();

			const char* module = exceptionFunction->GetModuleName();
			int line = context->GetExceptionLineNumber();
			const char* description = context->GetExceptionString();

			CRITICAL("Error occured while calling function \"" + functionName + "\" in module \"" + std::string(module) + "\" from namespace\"" + moduleName + "\" at line {" + std::to_string(line) + "} with description \"" + std::string(description) + "\"");
		} else {
			CRITICAL("Error occured while calling function \"" + functionName + "\" in module from namespace \"" + moduleName + "\" with result enum of {" + std::to_string(executionResult) + "}, aborting");
		}
		context->Release();
		psnip_trap();
	}
}

void ModHandler::CallModFunction(asIScriptFunction* function) {
	OVERRIDE_LOG_NAME("Mod Script Execution");
	asIScriptContext* context = engine->CreateContext();
	context->Prepare(function);

	int result = context->Execute();
	if (result != asEXECUTION_FINISHED) {
		if (result == asEXECUTION_EXCEPTION) {
			asIScriptFunction* exceptionFunction = context->GetExceptionFunction();

			const char* module = exceptionFunction->GetModuleName();
			int line = context->GetExceptionLineNumber();
			const char* description = context->GetExceptionString();

			CRITICAL("Error occured while calling function \"" + std::string(function->GetName()) + "\" in module \"" + std::string(module) + "\" at line {" + std::to_string(line) + "} with description \"" + std::string(description) + "\"");
		} else {
			CRITICAL("Error occured while calling function \"" + std::string(function->GetName()) + "\" with result enum of {" + std::to_string(result) + "}, aborting");
		}
		psnip_trap();
	}

	context->Release();
}

std::string ModHandler::LoadFile(std::string path) {
	std::ifstream file(path);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void ModHandler::ModError(const asSMessageInfo* messageInfo, void* parameter) {
	std::ostream* out = reinterpret_cast<std::ostream*>(parameter);
	*out << messageInfo->section << " (" << messageInfo->row << ", " << messageInfo->col << ") : "
         << messageInfo->message << std::endl;
}

void ModHandler::ModLog(const std::string& message) {
	OVERRIDE_LOG_NAME("Mod script");
	EXT(message);
}

void ModHandler::Delete() {
    //m3_FreeRuntime(modRuntime);
    //m3_FreeEnvironment(modEnvironment);
}