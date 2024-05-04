#version 330 core

layout (location = 0) in vec3 blockPosition;
layout (location = 1) in vec2 textureCoordinate;

out vec2 textureCoordinateOut;

uniform mat4 windowViewMatrix;

void main()
{
	vec4 worldPosition = vec4(blockPosition, 1.0);

	gl_Position = windowViewMatrix * worldPosition;
	textureCoordinateOut = textureCoordinate;
}

//#version 330 core
//layout(location = 0) in vec3 aPos;
//uniform mat4 windowViewMatrix;
//void main() {
//	gl_Position = windowViewMatrix * vec4(aPos, 1.0);
//}