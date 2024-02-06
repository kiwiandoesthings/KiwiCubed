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
	private:
		int xPos;
		int yPos;
		int zPos;

		bool isSolid;

	public:
		static const int vertexSize = 5; // 3 for position, 2 for texture coordinates

		// Default constructor
		Block() : xPos(0), yPos(0), zPos(0) {}
		Block(int xPos, int yPos, int zPos);

		bool IsSolid(int x, int y, int z, int CHUNK_SIZE);
		void SetSolid(bool solid);

		// Method to generate mesh data for the block
		void GenerateMesh(int x, int y, int z, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, FaceDirection faceDirection);
		void addFace(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, int x, int y, int z, FaceDirection faceDir);
};