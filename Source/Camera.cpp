#include <Camera.h>


Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgramID, uniform), 1, GL_FALSE, glm::value_ptr(CameraMatrix)));
}

void Camera::UpdateMatrix(Window* window, float FOV, float nearPlane, float farPlane)
{
	width = window->GetWidth();
	height = window->GetHeight();

	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 projectionMatrix = glm::mat4(1.0f);

	viewMatrix = glm::lookAt(Position, Position + Orientation, UpDirection);
	projectionMatrix = glm::perspective(glm::radians(FOV), static_cast<float>(width) / static_cast<float>(height), nearPlane, farPlane);

	CameraMatrix = projectionMatrix * viewMatrix;
}

void Camera::SetPosition(float cameraX, float cameraY, float cameraZ) {
	Position = glm::vec3(cameraX, cameraY, cameraZ);
}

void Camera::Inputs(Window* window)
{
	GLFWwindow* windowInstance = window->GetWindowInstance();
	if (window->isFocused) {
		if (glfwGetKey(windowInstance, GLFW_KEY_W) == GLFW_PRESS)
		{
			Position += speed * Orientation;
		}
		if (glfwGetKey(windowInstance, GLFW_KEY_A) == GLFW_PRESS)
		{
			Position += speed * -glm::normalize(glm::cross(Orientation, UpDirection));
		}
		if (glfwGetKey(windowInstance, GLFW_KEY_S) == GLFW_PRESS)
		{
			Position += speed * -Orientation;
		}
		if (glfwGetKey(windowInstance, GLFW_KEY_D) == GLFW_PRESS)
		{
			Position += speed * glm::normalize(glm::cross(Orientation, UpDirection));
		}
		if (glfwGetKey(windowInstance, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			Position += speed * UpDirection;
		}
		if (glfwGetKey(windowInstance, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			Position += speed * -UpDirection;
		}
		if (glfwGetKey(windowInstance, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			speed = 0.4f;
		}
		else if (glfwGetKey(windowInstance, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
		{
			speed = 0.4f;
		}
	}

	if (window->isFocused) {
		double mouseX;
		double mouseY;

		// Fetches the coordinates of the cursor
		glfwGetCursorPos(windowInstance, &mouseX, &mouseY);

		float rotX = sensitivity * (float)(mouseY - (window->GetHeight() / 2)) / window->GetHeight();
		float rotY = sensitivity * (float)(mouseX - (window->GetWidth() / 2)) / window->GetWidth();

		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, UpDirection)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, UpDirection) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		Orientation = glm::rotate(Orientation, glm::radians(-rotY), UpDirection);

		glfwSetCursorPos(windowInstance, (window->GetWidth() / 2), (window->GetHeight() / 2));
	}
}