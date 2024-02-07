#pragma once

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <Shader.h>

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 UpDirection = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 CameraMatrix = glm::mat4(1.0f);

	bool firstClick = true;
	bool cursorFree = true;

	int width;
	int height;

	float speed = 0.004f;
	float sensitivity = 100.0f;

	Camera(int width, int height, glm::vec3 position);

	void Matrix(Shader& shader, const char* uniform);
	void UpdateMatrix(float FOV, float nearPlane, float farPlane);
	void SetPosition(float cameraX, float cameraY, float cameraZ);
	void Inputs(GLFWwindow* window);
	void UpdateWindowSize(int windowWidth, int windowHeight);
};