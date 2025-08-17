#version 330 core

out vec4 FragColor;

in vec2 textureCoordinateOut;


void main()
{
    FragColor = vec4(textureCoordinateOut.x, textureCoordinateOut.y, textureCoordinateOut.x + textureCoordinateOut.y, 1);
}