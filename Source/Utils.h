#pragma once

struct Vec3 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Vec3 operator*(Vec3& other) {
		return Vec3(x * other.x, y * other.y, z * other.z);
	}
	Vec3 operator/(Vec3& other) {
		return Vec3(x / other.x, y / other.y, z / other.z);
	}
	Vec3 operator-(Vec3& other) {
		return Vec3(x - other.x, y - other.y, z - other.z);
	}
	Vec3 operator+(Vec3& other) {
		return Vec3(x + other.x, y + other.y, z + other.z);
	}
	bool operator==(Vec3& other) {
		return x == other.x && y == other.y && z == other.z;
	}
	Vec3& operator=(Vec3& other) {
		x = other.x;
    	y = other.y;
    	z = other.z;
   		return *this;
	}

	Vec3(int newX, int newY, int newZ) : x(static_cast<float>(newX)), y(static_cast<float>(newY)), z(static_cast<float>(newZ)) {}
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};