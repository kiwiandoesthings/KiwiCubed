extern "C" {
    struct KC_TextureAtlasData {
        unsigned short variant;
        unsigned char xPosition;
        unsigned char yPosition;
        unsigned char xSize;
        unsigned char ySize;
    };

    void KC_Log(const char* message);
    void KC_AddEventToDo(const char* eventName, const char* functionName);
    unsigned int KC_GetTextureNumericalID(const char* modName, const char* assetName);

    void UpdateItemEntity() {
        KC_Log("Item entity update");
    }
    
    void PlayerMinedBlock() {
        KC_Log("Player mined a block");
    }
    void RegisterWorldEventHooks() {
        KC_AddEventToDo("event/player_mined_block", "PlayerMinedBlock");
    }

    void Initialize() {
        KC_Log("Initializing vanilla KiwiCubed mod");

        KC_AddEventToDo("event/generate_world", "RegisterWorldEventHooks");
        
        unsigned int itemTextureID = KC_GetTextureNumericalID("kiwicubed", "texture/stone");

        KC_Log("Finished initialization");
    }
}