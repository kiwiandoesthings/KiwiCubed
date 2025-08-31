#version 330 core

out vec4 FragColor;

in vec2 textureCoordinatesOut;
flat in int generationStatusOut;

uniform sampler2D tex0;
uniform vec2 atlasSize;

float epsilon = 0.00001;


void main()
{
    float textureIndexX = mod(generationStatusOut + 4 - 1 + epsilon, atlasSize.x);
    float textureIndexY = floor((generationStatusOut + 4 - 1 + epsilon) / atlasSize.y);

    vec2 newTextureCoordinates = vec2(
        (textureCoordinatesOut.x / float(atlasSize.x)) + (textureIndexX / float(atlasSize.x)),
        (textureCoordinatesOut.y / float(atlasSize.y)) + (textureIndexY / float(atlasSize.y))
    );
    
    vec4 baseColor = texture(tex0, newTextureCoordinates);

    FragColor = baseColor;
}
