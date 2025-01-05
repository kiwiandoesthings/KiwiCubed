#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;


void main()
{
	vec4 worldPosition = vec4(position, 1);

	gl_Position = projectionMatrix * viewMatrix * worldPosition;
}