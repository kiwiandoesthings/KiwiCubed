#version 330 core

out vec4 FragColor;

in vec2 textureCoordinateOut;
flat in int generationStatusOut;

uniform sampler2D tex0;
uniform uint atlasSize;


void main()
{
    float epsilon = 0.00001;

    float textureIndexX = mod(generationStatusOut + 4 - 1 + epsilon, atlasSize);
    float textureIndexY = floor((generationStatusOut + 4 - 1 + epsilon) / atlasSize);

    vec2 offset = vec2(textureIndexX, textureIndexY);
    vec2 newTextureCoordinate = vec2(
        (textureCoordinateOut.x / atlasSize) + (offset.x / atlasSize), 
        (textureCoordinateOut.y / atlasSize) + (offset.y / atlasSize)
    );
    
    vec4 baseColor = texture(tex0, newTextureCoordinate);

    FragColor = baseColor;
}
