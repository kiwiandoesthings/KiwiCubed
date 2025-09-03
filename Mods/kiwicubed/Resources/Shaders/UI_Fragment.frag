#version 330 core

out vec4 FragColor;

in vec2 textureCoordinatesOut;

uniform sampler2D tex0;
uniform vec2 atlasSize;
uniform vec2 textureSize;
uniform vec2 borderSize;
uniform vec2 elementDimensions;
uniform uint textureIndex;

const float epsilon = 0.00001;


void main()
{
    float textureIndexX = mod(textureIndex + 3u + epsilon, atlasSize.x);
    float textureIndexY = floor((textureIndex + 3u + epsilon) / atlasSize.y);
    textureIndexY = 0;

    vec2 newTextureCoordinates = vec2(
        (textureCoordinatesOut.x / float(atlasSize.x)) + (textureIndexX / float(atlasSize.x)),
        (textureCoordinatesOut.y / float(atlasSize.y)) + (textureIndexY / float(atlasSize.y))
    );

    vec2 borders = borderSize / textureSize;
    vec2 dimensions = borderSize / elementDimensions * borderSize;

    //newTextureCoordinates = vec2(
    //    processAxis(newTextureCoordinates.x, borders.x, dimensions.x),
    //    processAxis(newTextureCoordinates.y, borders.y, dimensions.y)
    //);
    
    FragColor = texture(tex0, newTextureCoordinates);
}