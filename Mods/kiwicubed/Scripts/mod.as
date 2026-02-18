void Initialize() {
    Log("Initializing KiwiCubed default mod");

	AssetStringID itemID("kiwicubed", "entity/dropped_item");
	RegisterFunctionToEvent(EVENT_WORLD_PLAYER_BLOCK);
	RegisterEventToEntityType(EVENT_WORLD_TICK, itemID);
	RegisterEventToEntityType(EVENT_WORLD_PLAYER_MOVE, itemID);
	
	Log("Finished initializing KiwiCubed default mod");
}

void GetEventWorldPlayerBlock(const EventWorldPlayerBlock &in event) {
	AssetStringID item("kiwicubed", "entity/dropped_item");
	AssetStringID model("kiwicubed", "model/dropped_item");
	AssetStringID atlas("kiwicubed", "terrain_atlas");
	uint64 id = SpawnEntity(item, model, atlas, GetBlockTextureAtFace(event.oldBlockStringID, 0), float((event.chunkX * 32) + event.blockX + 0.5), float((event.chunkY * 32) + event.blockY + 0.15), float((event.chunkZ * 32) + event.blockZ + 0.5));
	itemOriginalY["" + id] = float((event.chunkY * 32) + event.blockY) + 0.15;
	itemBlock["" + id] = event.oldBlockStringID;
}

void EntityGetEventWorldTick(const EventWorldTick &in event, const uint64 id) {
	EntityTransform transform = GetEntityTransform(id);

	Vec3 orientation = transform.GetOrientation();
	orientation.y += 2;
	transform.SetOrientation(orientation);
	Vec3 position = transform.GetPosition();
	float originalY = 0.0f;
	itemOriginalY.get("" + id, originalY);
	position.y = originalY + ((sin(float(event.tickNumber) / 5.0f)) + 1) * 0.08;
	transform.SetPosition(position);

	SetEntityTransform(id, transform);
}

void EntityGetEventWorldPlayerMove(const EventWorldPlayerMove &in event, const uint64 id) {
	EntityTransform transform = GetEntityTransform(id);
	Vec3 position = transform.GetPosition();
	if (GetDistanceSquared(Vec3(event.newPlayerX, event.newPlayerY, event.newPlayerZ), Vec3(position.x, position.y, position.z)) < 0.25) {
		AssetStringID blockStringID;
		itemBlock.get("" + id, blockStringID);
		AddItem(blockStringID, 1, event.playerAUID);
		RemoveEntity(id);
	}
}

float GetDistanceSquared(Vec3 p1, Vec3 p2) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float dz = p2.z - p1.z;
    return dx*dx + dy*dy + dz*dz;
}
dictionary itemOriginalY;
dictionary itemBlock;