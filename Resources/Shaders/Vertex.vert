#version 330 core

layout (location = 0) in vec3 blockPosition;
layout (location = 1) in vec2 textureCoordinate;

out vec2 textureCoordinateOut;
out vec3 blockPositionOut;

uniform mat4 windowViewMatrix;

void main()
{
	vec4 worldPosition = vec4(blockPosition, 1.0);

	gl_Position = windowViewMatrix * worldPosition;
	textureCoordinateOut = textureCoordinate;
	blockPositionOut = blockPosition;
}