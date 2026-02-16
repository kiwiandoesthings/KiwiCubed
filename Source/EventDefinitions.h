#pragma once

#include "AssetDefinitions.h"


struct EventMetaWindowResize {
	int newWidth;
	int newHeight;
};

struct EventWorldTick {
	unsigned long long tickNumber;
};

struct EventWorldPlayerBlock {
	BlockEventType blockEventType;
	unsigned long long playerAUID;
	int chunkX;
	int chunkY;
	int chunkZ;
	int blockX;
	int blockY;
	int blockZ;
	AssetStringID oldBlockStringID;
	AssetStringID newBlockStringID;
};

struct EventWorldPlayerMove {
	unsigned long long playerAUID;
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

struct EventWorldEntityBlock {
	BlockEventType blockEventType;
	unsigned long long entityAUID;
	int chunkX;
	int chunkY;
	int chunkZ;
	int blockX;
	int blockY;
	int blockZ;
	AssetStringID oldBlockStringID;
	AssetStringID newBlockStringID;
};

struct EventWorldEntityHurt {
	unsigned long long damagedAUID;
};

struct EventWorldEntityAttack {
	unsigned long long attackerAUID;
	unsigned long long attackedAUID;
};

struct EventWorldGenericBlock {
	BlockEventType blockEventType;
	int chunkX;
	int chunkY;
	int chunkZ;
	int blockX;
	int blockY;
	int blockZ;
	AssetStringID oldBlockStringID;
	AssetStringID newBlockStringID;
};