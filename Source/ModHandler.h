#pragma once

#include <klogger.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include "Block.h"
#include "TextureManager.h"

using json = nlohmann::json;


class ModHandler {
    public:
        ModHandler();
        ~ModHandler();

       bool SetupTextureAtlasData(); 

    private:
    
};