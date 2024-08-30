#version 330 core

out vec4 FragColor;

in vec3 blockPositionOut;
in vec2 textureCoordinateOut;
flat in uint blockTypeOut;

uniform sampler2D tex0;

void main()
{

	vec3 tint = vec3(mod(blockPositionOut.x, 32) * 2, mod(blockPositionOut.y, 32) * 2, mod(blockPositionOut.z, 32) * 2);
    tint = ((tint + vec3(1.0)) / 255.0 * 10);


	vec2 newTextureCoordinate = vec2(textureCoordinateOut.x / 2, textureCoordinateOut.y / 2);
	vec4 baseColor = texture(tex0, newTextureCoordinate);
    FragColor = baseColor * vec4(tint, 0);
}