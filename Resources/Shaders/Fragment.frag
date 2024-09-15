#version 330 core

out vec4 FragColor;

in vec3 blockPositionOut;
in vec2 textureCoordinateOut;
in float blockTypeOut;

uniform sampler2D tex0;

float normalize(float value) {
	return ((value * 16 ) / 32);
}

void main()
{

	//vec3 tint = vec3(mod(blockPositionOut.x, 32) * 2, mod(blockPositionOut.y, 32) * 2, mod(blockPositionOut.z, 32) * 2);
    //tint = ((tint + vec3(1.0)) / 255.0 * 10);

	//vec2 atlasTexCoord = textureCoordinateOut * vec2(0.5, 0.5);  // Scale to the size of the tile
    //atlasTexCoord += vec2(blockTypeOut * 1, blockTypeOut * 1);  // Offset to the tile position

	vec2 offset = vec2(normalize(blockTypeOut), normalize(blockTypeOut));
	vec2 newTextureCoordinate = vec2(normalize(textureCoordinateOut.x) + offset.x, normalize(textureCoordinateOut.y) + offset.y);
	vec4 baseColor = texture(tex0, newTextureCoordinate);
    FragColor = baseColor/* * vec4(tint, 0)*/;
}