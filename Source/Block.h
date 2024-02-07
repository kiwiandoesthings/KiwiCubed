#pragma once

#include <glad/glad.h>

#include <iostream>
#include <vector>


enum FaceDirection {
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};


class Block {
	public:
		static const int vertexSize = 5; // 3 for position, 2 for texture coordinates

		// Default constructor
		Block() : blockX(0), blockY(0), blockZ(0), isSolid(-1) {}
		Block(int blockX, int blockY, int blockZ);

		void GenerateBlock (int blockX, int blockY, int blockZ, int chunkX, int chunkY, int chunkZ, int chunkSize);
		void SetSolid(int solid);
		int GetSolid();

		// Method to generate mesh data for the block
		void GenerateMesh(int x, int y, int z, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, FaceDirection faceDirection);
		void AddFace(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, int x, int y, int z, FaceDirection faceDir);

	private:
		int blockX;
		int blockY;
		int blockZ;

		int isSolid;

};