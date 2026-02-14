extern "C" {
    struct KC_TextureAtlasData {
        unsigned short variant;
        unsigned char xPosition;
        unsigned char yPosition;
        unsigned char xSize;
        unsigned char ySize;
    };

    void KC_Log(const char* message);
    unsigned int KC_GetTextureNumericalID(const char* modName, const char* assetName);

    void UpdateItemEntity() {
        KC_Log("Item entity update");
    }
    
    void PlayerMinedBlock() {
        KC_Log("Player mined a block");
    }
    void RegisterWorldEventHooks() {
    }

    void Initialize() {
        KC_Log("Initializing vanilla KiwiCubed mod");
        
        unsigned int itemTextureID = KC_GetTextureNumericalID("kiwicubed", "texture/stone");

        KC_Log("Finished initialization");
    }
}