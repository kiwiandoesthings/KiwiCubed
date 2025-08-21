#version 330 core

out vec4 FragColor;

in vec2 textureCoordinatesOut;

uniform sampler2D tex0;
uniform uint atlasSize;
//uniform float borderSize;
//uniform vec2 textureSize;

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
    float textureIndexX = 0;
    float textureIndexY = 0;

    //FragColor = texture(tex0, vec2(
    //    (textureCoordinatesOut.x / float(atlasSize)) + (textureIndexX / float(atlasSize)),
    //    (textureCoordinatesOut.y / float(atlasSize)) + (textureIndexY / float(atlasSize))
    //));

    float borderSize = 1;
    vec2 textureSize = vec2(64, 64);

    vec2 borders = vec2(borderSize) / textureSize.xx;

    vec2 newUV = vec2(
        processAxis(textureCoordinatesOut.x, 1, borders.x),
        processAxis(textureCoordinatesOut.y, 1, borders.y)
    );
    
    FragColor = texture(tex0, newUV);
}