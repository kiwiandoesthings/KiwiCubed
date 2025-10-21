#version 330 core

out vec4 FragColor;

in vec2 textureCoordinatesOut;

uniform sampler2D tex0;
uniform vec2 atlasSize;

const float epsilon = 0.00001;


void main()
{
    FragColor = texture(tex0, textureCoordinatesOut);
}