#version 330 core

out vec4 FragColor;

in vec3 blockPositionOut;
in vec2 textureCoordinateOut;
in float blockTypeOut;

uniform sampler2D tex0;
uniform uint atlasSize;


void main()
{
	//vec3 tint = vec3(mod(blockPositionOut.x, 32) * 2, mod(blockPositionOut.y, 32) * 2, mod(blockPositionOut.z, 32) * 2);
    //tint = ((tint + vec3(1.0)) / 255.0 * 10);
    vec3 tint = vec3(1.0, 1.0, 1.0)

    float epsilon = 0.00001;

    float textureIndexX = mod(blockTypeOut - 1 + epsilon, atlasSize);
    float textureIndexY = floor((blockTypeOut - 1 + epsilon) / atlasSize);

    vec2 offset = vec2(textureIndexX, textureIndexY);
    vec2 newTextureCoordinate = vec2(
        (textureCoordinateOut.x / atlasSize) + (offset.x / atlasSize), 
        (textureCoordinateOut.y / atlasSize) + (offset.y / atlasSize)
    );

	vec4 baseColor = texture(tex0, newTextureCoordinate);
    FragColor = baseColor * tint;
}