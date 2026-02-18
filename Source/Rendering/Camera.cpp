#include "Camera.h"
#include "glm/ext.hpp"

void Camera::SetCameraMatrix(Shader& shader) const {
	shader.Bind();
	shader.SetUniformMatrix4fv("projectionMatrix", projectionMatrix);
	shader.SetUniformMatrix4fv("viewMatrix", viewMatrix);
}

void Camera::UpdateMatrix(float FOV, float nearPlane, float farPlane, glm::vec3 position, glm::vec3 orientation, glm::vec3 upDirection) {
	int width = window.GetWidth();
	int height = window.GetHeight();
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

	viewMatrix = glm::lookAt(position, position + orientation, upDirection);
	projectionMatrix = glm::perspective(glm::radians(FOV), aspectRatio, nearPlane, farPlane);
	frustum.Update(projectionMatrix * viewMatrix);
}

Window& Camera::GetWindow() {
	return window;
}
