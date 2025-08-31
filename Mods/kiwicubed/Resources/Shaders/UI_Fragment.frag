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

float map(float value, float originalMin, float originalMax, float newMin, float newMax) {
    return (value - originalMin) / (originalMax - originalMin) * (newMax - newMin) + newMin;
} 

float processAxis(float coord, float textureBorder, float windowBorder) {
    if (coord < windowBorder)
        return map(coord, 0, windowBorder, 0, textureBorder) ;
    if (coord < 1 - windowBorder) 
        return map(coord,  windowBorder, 1 - windowBorder, textureBorder, 1 - textureBorder);
    return map(coord, 1 - windowBorder, 1, 1 - textureBorder, 1);
}


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