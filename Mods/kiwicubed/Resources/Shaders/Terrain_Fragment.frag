#version 330 core

out vec4 FragColor;

in vec3 blockPositionOut;
in vec2 textureCoordinatesOut;

uniform sampler2D tex0;
uniform vec2 atlasSize;

const float epsilon = 0.00001;


void main() 
{
    //vec3 tint = vec3(mod(blockPositionOut.x, 32) * 2, mod(blockPositionOut.y, 32) * 2, mod(blockPositionOut.z, 32) * 2);
    //tint = ((tint + vec3(1.0)) / 255.0 * 10);
    vec3 tint = vec3(1.0, 1.0, 1.0);

    FragColor = texture(tex0, textureCoordinatesOut) * vec4(tint, 1.0);
}
