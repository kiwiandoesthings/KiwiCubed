void Initialize() {
    Log("Initializing KiwiCubed default mod");

	AssetStringID itemID("kiwicubed", "entity/dropped_item");
	RegisterFunctionToEvent(EVENT_WORLD_PLAYER_BLOCK);
	
	Log("Finished initializing KiwiCubed default mod");
}

void GetEventWorldPlayerBlock(const EventWorldPlayerBlock &in event) {
	AssetStringID item("kiwicubed", "entity/dropped_item");
	AssetStringID model("kiwicubed", "model/dropped_item");
	AssetStringID atlas("kiwicubed", "terrain_atlas");
	SpawnEntity(item, model, atlas, GetBlockTextureAtFace(event.oldBlockStringID, 0), float((event.chunkX * 32) + event.blockX + 0.5), float((event.chunkY * 32) + event.blockY + 0.15), float((event.chunkZ * 32) + event.blockZ + 0.5));
}

//entityTransform.orientation.y += 150.0f * Globals::GetInstance().deltaTime;
//entityRenderData.positionOffset.y = sin(world->GetTotalTicks() / 3.0f) * 0.025;