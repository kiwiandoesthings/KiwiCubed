#include "Block.h"
#include "Player.h"


struct MetaWindowResizeEvent {
	int newWidth;
	int newHeight;
};

struct WorldTickEvent {
	int tickNumber;
};

struct WorldPlayerBlockEvent {
	BlockEventType blockEventType;
	Player* player;
	int chunkX;
	int chunkY;
	int chunkZ;
	int blockX;
	int blockY;
	int blockZ;
	BlockType* oldBlock;
	BlockType* newBlock;
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
	Entity* entity;
	int chunkX;
	int chunkY;
	int chunkZ;
	int blockX;
	int blockY;
	int blockZ;
	BlockType* oldBlock;
	BlockType* newBlock;
};

struct WorldGenericBlockEvent {
	BlockEventType blockEventType;
	int chunkX;
	int chunkY;
	int chunkZ;
	int blockX;
	int blockY;
	int blockZ;
	BlockType* oldBlock;
	BlockType* newBlock;
};