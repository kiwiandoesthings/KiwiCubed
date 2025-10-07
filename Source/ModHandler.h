#pragma once

#include <klogger.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>
#include <../Vendor/robin_hood.h>

using json = nlohmann::json;


struct AssetStringID {
    std::string modName = "";
    std::string assetName = "";

    std::string CanonicalName() const {
        return modName + ":" + assetName;
    }

    bool operator==(const AssetStringID& other) const {
        return modName == other.modName && assetName == other.assetName;
    }
};

template <>
struct std::hash<AssetStringID>{
    std::size_t operator()(const AssetStringID& stringID) const {
        return std::hash<std::string>()(stringID.modName + ":" + stringID.assetName);
    }
};

class ModHandler {
    public:
        ModHandler();
        ~ModHandler();

       bool SetupTextureAtlasData(); 
};