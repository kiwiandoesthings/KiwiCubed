#pragma once

#include <klogger.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>
#include <robin_hood.h>
#include <wasm3.h>
#include <m3_env.h>

#include "Events.h"
#include "ExposedModAPI.h"


using orderedJson = nlohmann::ordered_json;


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
        return std::hash<std::string>()(stringID.modName + ":" + stringID.assetName);
    }
};


class ModHandler {
    public:
        static ModHandler& GetInstance();
        void Delete();

        bool SetupTextureAtlasData(); 
        bool LoadModScripts();
        bool RunModEntrypoints();

        void SetCurrentlyProcessingEvent(Event* event);
        void CallWasmFunction(std::string functionName);

    private:
        ModHandler() = default;
        ~ModHandler() = default;

        ModHandler(const ModHandler&) = delete;
        ModHandler& operator=(const ModHandler&) = delete;

        std::vector<std::string> modNamespaces;
        std::unordered_map<std::string, std::string> modNamespacesToScripts;
        IM3Environment modEnvironment = m3_NewEnvironment();
        IM3Runtime modRuntime = m3_NewRuntime(modEnvironment, 512 * 1024, nullptr);
        std::vector<std::vector<uint8_t>> wasmModBuffers;

        Event* currentlyProcessingEvent = nullptr;

        static std::vector<uint8_t> LoadFile(std::string path);
};