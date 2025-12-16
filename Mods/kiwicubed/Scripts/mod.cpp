extern "C" {
    void KC_Log(const char* message);
    void KC_AddEventToDo(const char* eventName, const char* functionName);
    
    void PlayerMinedBlock() {
        KC_Log("Player mined a block");
    }
    void RegisterWorldEventHooks() {
        KC_AddEventToDo("event/player_mined_block", "PlayerMinedBlock");
    }

    void Initialize() {
        KC_Log("Initializing vanilla KiwiCubed mod");

        KC_AddEventToDo("event/generate_world", "RegisterWorldEventHooks");

        KC_Log("Finished initialization");
    }
}