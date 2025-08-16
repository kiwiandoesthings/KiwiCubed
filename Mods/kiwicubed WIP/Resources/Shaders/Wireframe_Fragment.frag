#version 330 core

in vec3 worldPositionOut;

out vec4 FragColor;

uniform vec3 cameraPosition;


void main()
{
    vec3 tint = ((worldPositionOut - cameraPosition) + 0.5) * 255;

    FragColor = vec4(tint, 1.0);
}