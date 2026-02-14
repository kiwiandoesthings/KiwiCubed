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

#include "ExposedModAPI.h"


using OrderedJSON = nlohmann::ordered_json;


class ModHandler {
    public:
        static ModHandler& GetInstance();
        void Delete();

        bool LoadModData(); 
        bool LoadModScripts();
        bool RunModEntrypoints();

        void CallWasmFunction(std::string functionName, const void* arguments[]);

    private:
        ModHandler() = default;
        ~ModHandler() = default;

        ModHandler(const ModHandler&) = delete;
        ModHandler& operator=(const ModHandler&) = delete;

        std::vector<std::string> modNamespaces;
        std::unordered_map<std::string, std::string> modNamespacesToScripts;
        std::unordered_map<std::string, std::vector<std::string>> modToEntityCallbacks;
        IM3Environment modEnvironment = m3_NewEnvironment();
        IM3Runtime modRuntime = m3_NewRuntime(modEnvironment, 512 * 1024, nullptr);
        std::vector<std::vector<uint8_t>> wasmModBuffers;

        static std::vector<uint8_t> LoadFile(std::string path);
};