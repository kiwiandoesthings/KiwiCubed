#include <Block.h>

	
unsigned int Block::GetBlockID() const {
	return blockID;
}

void Block::SetBlockID(unsigned int newBlockID) {
	blockID = newBlockID;
}

unsigned short Block::GetVariant() const {
	return variant;
}

void Block::SetVariant(unsigned short newVariant) {
	variant = newVariant;
}

bool Block::IsAir() {
	return blockID == 0;
}