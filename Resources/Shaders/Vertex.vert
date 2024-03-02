#version 330 core

layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
layout (location = 1) in vec2 aTex;

//out vec3 color;
out vec2 texCoord;

uniform mat4 windowViewMatrix;
//uniform vec3 chunkPosition;

void main()
{
	vec4 worldPosition = vec4(aPos, 1.0);
	worldPosition.x += /*chunkPosition.x * 32*/ 0;
	worldPosition.y += /*chunkPosition.y * 32*/ 0;
	worldPosition.z += /*chunkPosition.z * 32*/ 0;

	gl_Position = windowViewMatrix * worldPosition;
	texCoord = aTex;
}