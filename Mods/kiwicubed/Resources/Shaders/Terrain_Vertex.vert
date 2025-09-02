#version 330 core

layout (location = 0) in vec3 blockPosition;
layout (location = 1) in vec2 textureCoordinates;

out vec2 textureCoordinatesOut;
out vec3 blockPositionOut;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;


void main()
{
	vec4 worldPosition = vec4(blockPosition, 1);

	gl_Position = projectionMatrix * viewMatrix * worldPosition;
	blockPositionOut = blockPosition;
	textureCoordinatesOut = textureCoordinates;
}