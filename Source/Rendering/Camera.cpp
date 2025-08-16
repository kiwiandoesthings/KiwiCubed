#include "Camera.h"
#include "glm/ext.hpp"

void Camera::Setup(Window& window) {
	Camera::window = window;
	windowInstance = window.GetWindowInstance();
}

void Camera::SetCameraMatrix(Shader& shader) const {
	shader.Bind();
	shader.SetUniform4fv("projectionMatrix", projectionMatrix);
	shader.SetUniform4fv("viewMatrix", viewMatrix);
}

void Camera::UpdateMatrix(float FOV, float nearPlane, float farPlane, glm::vec3 position, glm::vec3 orientation, glm::vec3 upDirection) {
	int width = window.GetWidth();
	int height = window.GetHeight();
	
	viewMatrix = glm::lookAt(position, position + orientation, upDirection);
	projectionMatrix = glm::perspective(glm::radians(FOV), static_cast<float>(width) / static_cast<float>(height), nearPlane, farPlane);
}

Window& Camera::GetWindow() {
	return window;
}
