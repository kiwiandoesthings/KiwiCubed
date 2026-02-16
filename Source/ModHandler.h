#pragma once

#include <klogger.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <angelscript.h>
#include <nlohmann/json.hpp>
#include <robin_hood.h>
#include <scriptstdstring.h>

#include "Utils.h"


using OrderedJSON = nlohmann::ordered_json;


class ModHandler {
    public:
        static ModHandler& GetInstance();
        void Delete();

        bool LoadModData(); 
        bool LoadModScripts();
        bool RunModEntrypoints();

		void CallModFunction(const std::string& moduleName, const std::string& functionName, const std::vector<void*>& arguments, const std::vector<int>& argumentTypes);
		void CallModFunction(asIScriptFunction* function);

    private:
        ModHandler() = default;
        ~ModHandler() = default;

        ModHandler(const ModHandler&) = delete;
        ModHandler& operator=(const ModHandler&) = delete;

        std::vector<std::string> modNamespaces;
        std::unordered_map<std::string, std::string> modNamespacesToScripts;
        std::unordered_map<std::string, std::vector<std::string>> modToEntityCallbacks;

		asIScriptEngine* engine = nullptr;
		std::unordered_map<std::string, asIScriptModule*> modNamespacesToModules;

		static std::string LoadFile(std::string path);
		static void ModError(const asSMessageInfo* messageInfo, void* parameter);
		static void ModLog(const std::string& message);
};