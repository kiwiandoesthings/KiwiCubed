#include <Camera.h>


Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgramID, uniform), 1, GL_FALSE, glm::value_ptr(CameraMatrix));
}

void Camera::UpdateMatrix(float FOV, float nearPlane, float farPlane)
{
	glm::mat4 viewProjectionMatrix = glm::mat4(1.0f);
	glm::mat4 projectionMatrix = glm::mat4(1.0f);

	viewProjectionMatrix = glm::lookAt(Position, Position + Orientation, UpDirection);
	projectionMatrix = glm::perspective(glm::radians(FOV), static_cast<float>(width) / static_cast<float>(height), nearPlane, farPlane);

	CameraMatrix = projectionMatrix * viewProjectionMatrix;
}

void Camera::Inputs(GLFWwindow* window)
{
	if (cursorFree == false) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			Position += speed * Orientation;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			Position += speed * -glm::normalize(glm::cross(Orientation, UpDirection));
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			Position += speed * -Orientation;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			Position += speed * glm::normalize(glm::cross(Orientation, UpDirection));
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			Position += speed * UpDirection;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			Position += speed * -UpDirection;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			speed = 0.04f;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
		{
			speed = 0.04f;
		}
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			cursorFree = true;
		}
	}

	double mouseX;
	double mouseY;
	// Fetches the coordinates of the cursor
	glfwGetCursorPos(window, &mouseX, &mouseY);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && cursorFree)
	{
		cursorFree = false;
	}
	if (!cursorFree && !firstClick)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		glfwSetCursorPos(window, (width / 2), (height / 2));
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, UpDirection)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, UpDirection) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), UpDirection);
	}
	if (!cursorFree && firstClick)
	{
		glfwSetCursorPos(window, width / 2, height / 2);
		firstClick = false;
	}
	if (cursorFree)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}

	// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
}

void Camera::UpdateWindowSize(int windowWidth, int windowHeight)
{
	Camera::width = windowWidth;
	Camera::height = windowHeight;

	UpdateMatrix(80.0f, 0.1f, 1000.0f);
}