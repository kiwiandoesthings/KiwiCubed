#pragma once

#include <glad/glad.h>

#include "glm/glm.hpp"

#include "IndexBufferObject.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"


struct Plane {
	glm::vec3 normal;
	float distance;

	void Normalize() {
        float length = glm::length(normal);
        normal /= length;
        distance /= length;
    }

	float GetDistanceSigned(const glm::vec3& point) const {
		return glm::dot(normal, point) + distance;
	}
};

struct Frustum {
    Plane planes[6];

    void Update(const glm::mat4& viewProj) {
        planes[0].normal.x = viewProj[0][3] + viewProj[0][0];
        planes[0].normal.y = viewProj[1][3] + viewProj[1][0];
        planes[0].normal.z = viewProj[2][3] + viewProj[2][0];
        planes[0].distance = viewProj[3][3] + viewProj[3][0];

        planes[1].normal.x = viewProj[0][3] - viewProj[0][0];
        planes[1].normal.y = viewProj[1][3] - viewProj[1][0];
        planes[1].normal.z = viewProj[2][3] - viewProj[2][0];
        planes[1].distance = viewProj[3][3] - viewProj[3][0];

        planes[2].normal.x = viewProj[0][3] + viewProj[0][1];
        planes[2].normal.y = viewProj[1][3] + viewProj[1][1];
        planes[2].normal.z = viewProj[2][3] + viewProj[2][1];
        planes[2].distance = viewProj[3][3] + viewProj[3][1];

        planes[3].normal.x = viewProj[0][3] - viewProj[0][1];
        planes[3].normal.y = viewProj[1][3] - viewProj[1][1];
        planes[3].normal.z = viewProj[2][3] - viewProj[2][1];
        planes[3].distance = viewProj[3][3] - viewProj[3][1];

        planes[4].normal.x = viewProj[0][3] + viewProj[0][2];
        planes[4].normal.y = viewProj[1][3] + viewProj[1][2];
        planes[4].normal.z = viewProj[2][3] + viewProj[2][2];
        planes[4].distance = viewProj[3][3] + viewProj[3][2];

        planes[5].normal.x = viewProj[0][3] - viewProj[0][2];
        planes[5].normal.y = viewProj[1][3] - viewProj[1][2];
        planes[5].normal.z = viewProj[2][3] - viewProj[2][2];
        planes[5].distance = viewProj[3][3] - viewProj[3][2];

        for (int iterator = 0; iterator < 6; iterator++) {
            planes[iterator].Normalize();
        }
    }

    bool IsBoxVisible(const glm::vec3& min, const glm::vec3& max) const {
        for (int iterator = 0; iterator < 6; iterator++) {
            glm::vec3 point = min;
            if (planes[iterator].normal.x >= 0) point.x = max.x;
            if (planes[iterator].normal.y >= 0) point.y = max.y;
            if (planes[iterator].normal.z >= 0) point.z = max.z;

            if (planes[iterator].GetDistanceSigned(point) < 0) {
                return false;
            }
        }
        return true;
    }
};

struct Vertex {
	GLfloat position[3];
	GLfloat textureCoordinate[2];
};


class Renderer {
	public:
		Renderer() {};

		void ClearScreen(float redValue, float blueValue, float greenValue);
		void DrawElements(VertexArrayObject vertexArrayObject, VertexBufferObject vertexBufferObject, IndexBufferObject indexBufferObject, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices);

		void Delete() {};
};