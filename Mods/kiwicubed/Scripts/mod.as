void Initialize() {
    AssetStringID id("kiwicubed", "block/item");
	Log(id.CanonicalName());
	RegisterEventToEntityType(EVENT_WORLD_PLAYER_BLOCK, id);
}

void GetEventWorldPlayerBlock(const EventWorldPlayerBlock &in event) {
	Log("" + event.oldBlockStringID.CanonicalName());
}