#include <Camera.h>


Camera::Camera(int width, int height, glm::vec3 position) : position(glm::vec3(0, 0, 0)), pitch(0), yaw(0), roll(0) {
	Camera::width = width;
	Camera::height = height;
	position = position;
}

void Camera::Matrix(Shader& shader, const char* uniform) {
	// Sets the uniform in the shader to the camera's cameraMatrix value
	shader.SetUniform4fv(uniform, cameraMatrix);
}

void Camera::UpdateMatrix(Window* window, float FOV, float nearPlane, float farPlane) {
	// Updates the camera matrix w/ position and window size
	width = window->GetWidth();
	height = window->GetHeight();
	//std::cout << width << ": " << height << std::endl;

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
			speed = 0.2f;
		}
		else if (glfwGetKey(windowInstance, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
		{
			speed = 0.01f;
		}
	}

	if (window->isFocused) {
		// Does some absolute magic to rotate the camera correctly
		double mouseX;
		double mouseY;

		// Fetches the coordinates of the cursor
		glfwGetCursorPos(windowInstance, &mouseX, &mouseY);

		// Get the amount to rotate for the frame
		float rotationX = sensitivity * (float)(mouseY - (window->GetHeight() / 2)) / window->GetHeight();
		float rotationY = sensitivity * (float)(mouseX - (window->GetWidth() / 2)) / window->GetWidth();

		yaw += rotationY;
		pitch += rotationX;

		// Clamp pitch to prevent the camera from flipping out
		if (pitch > 89.9f)
			pitch = 89.9f;
		if (pitch < -89.9f)
			pitch = -89.9f;

		// wha..? (learnopengl.com)
		glm::vec3 facing;
		facing.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		facing.y = sin(glm::radians(-pitch));
		facing.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		orientation = glm::normalize(facing);

		// We don't want anyone to be able to move the mouse off the screen, that would be very very very bad and horrible and would make the game absolutely unplayable
		glfwSetCursorPos(windowInstance, (window->GetWidth() / 2), (window->GetHeight() / 2));
	}
}

Camera::~Camera() {

}