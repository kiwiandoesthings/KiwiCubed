#pragma once



struct MetaWindowResizeEvent {
	int newWidth;
	int newHeight;
};

struct WorldTickEvent {
	unsigned long long tickNumber;
};

struct WorldPlayerBlockEvent {
	BlockEventType blockEventType;
	unsigned long long playerAUID;
	int chunkX;
	int chunkY;
	int chunkZ;
	int blockX;
	int blockY;
	int blockZ;
	const char* oldBlockStringID;
	const char* newBlockStringID;
};

struct WorldPlayerMove {
	float oldPlayerX;
	float oldPlayerY;
	float oldPlayerZ;
	float oldPlayerYaw;
	float oldPlayerPitch;
	float oldPlayerRoll;
	float newPlayerX;
	float newPlayerY;
	float newPlayerZ;
	float newPlayerYaw;
	float newPlayerPitch;
	float newPlayerRoll;
};

struct WorldEntityBlockEvent {
	BlockEventType blockEventType;
	unsigned long long entityAUID;
	int chunkX;
	int chunkY;
	int chunkZ;
	int blockX;
	int blockY;
	int blockZ;
	const char* oldBlockStringID;
	const char* newBlockStringID;
};

struct WorldGenericBlockEvent {
	BlockEventType blockEventType;
	int chunkX;
	int chunkY;
	int chunkZ;
	int blockX;
	int blockY;
	int blockZ;
	const char* oldBlockStringID;
	const char* newBlockStringID;
};