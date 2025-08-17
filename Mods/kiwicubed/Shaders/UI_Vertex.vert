#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 textureCoordinate;

out vec2 textureCoordinateOut;

uniform mat4 modelMatrix;

void main()
{
    gl_Position = modelMatrix * vec4(position, 0, 1);

    textureCoordinateOut = textureCoordinate;
}