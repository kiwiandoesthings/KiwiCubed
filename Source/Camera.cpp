#include <Camera.h>


Camera::Camera(int width, int height, glm::vec3 position) : position(glm::vec3(0, 0, 0)) {
	Camera::width = width;
	Camera::height = height;
	position = position;
}

void Camera::Matrix(Shader& shader, const char* uniform) {
	// Sets the uniform in the shader to its cameraMatrix value
	GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgramID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix)));
}

void Camera::UpdateMatrix(Window* window, float FOV, float nearPlane, float farPlane) {
	// Updates the camera matrix w/ position and window size
	width = window->GetWidth();
	height = window->GetHeight();

	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 projectionMatrix = glm::mat4(1.0f);

	viewMatrix = glm::lookAt(position, position + orientation, upDirection);
	projectionMatrix = glm::perspective(glm::radians(FOV), static_cast<float>(width) / static_cast<float>(height), nearPlane, farPlane);

	cameraMatrix = projectionMatrix * viewMatrix;
}

void Camera::SetPosition(float cameraX, float cameraY, float cameraZ) {
	position = glm::vec3(cameraX, cameraY, cameraZ);
}

void Camera::Inputs(Window* window) {
	// Handles all inputs, TODO: make an api kind of thing for getting inputs eaisly, this WILL be procrastinated on
	GLFWwindow* windowInstance = window->GetWindowInstance();
	if (window->isFocused) {
		if (glfwGetKey(windowInstance, GLFW_KEY_W) == GLFW_PRESS)
		{
			position += speed * orientation;
		}
		if (glfwGetKey(windowInstance, GLFW_KEY_A) == GLFW_PRESS)
		{
			position += speed * -glm::normalize(glm::cross(orientation, upDirection));
		}
		if (glfwGetKey(windowInstance, GLFW_KEY_S) == GLFW_PRESS)
		{
			position += speed * -orientation;
		}
		if (glfwGetKey(windowInstance, GLFW_KEY_D) == GLFW_PRESS)
		{
			position += speed * glm::normalize(glm::cross(orientation, upDirection));
		}
		if (glfwGetKey(windowInstance, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			position += speed * upDirection;
		}
		if (glfwGetKey(windowInstance, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			position += speed * -upDirection;
		}
		if (glfwGetKey(windowInstance, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			speed = 0.04f;
		}
		else if (glfwGetKey(windowInstance, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
		{
			speed = 0.04f;
		}
	}

	if (window->isFocused) {
		// Does some absolute magic to rotate the camera correctly
		double mouseX;
		double mouseY;

		// Fetches the coordinates of the cursor
		glfwGetCursorPos(windowInstance, &mouseX, &mouseY);

		float rotX = sensitivity * (float)(mouseY - (window->GetHeight() / 2)) / window->GetHeight();
		float rotY = sensitivity * (float)(mouseX - (window->GetWidth() / 2)) / window->GetWidth();

		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, upDirection)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, upDirection) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			orientation = newOrientation;
		}

		orientation = glm::rotate(orientation, glm::radians(-rotY), upDirection);

		// We don't want anyone to be able to move the mouse off the screen, like in Roblox
		glfwSetCursorPos(windowInstance, (window->GetWidth() / 2), (window->GetHeight() / 2));
	}
}