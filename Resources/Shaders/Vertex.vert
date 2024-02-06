#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 texCoord;

uniform mat4 camMatrix;
uniform vec3 chunkPosition;

void main()
{
	vec4 worldPosition = vec4(aPos, 1.0);
	worldPosition.x += chunkPosition.x * 32;
	worldPosition.y += chunkPosition.y * 32;
	worldPosition.z += chunkPosition.z * 32;

	gl_Position = camMatrix * worldPosition;
	texCoord = aTex;
}