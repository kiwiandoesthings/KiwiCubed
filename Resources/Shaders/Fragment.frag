#version 330 core

out vec4 FragColor;

in vec2 textureCoordinateOut;
in vec3 blockPositionOut;

uniform sampler2D tex0;

void main()
{
	vec4 baseColor = texture(tex0, textureCoordinateOut);


	vec3 tint = vec3(mod(blockPositionOut.x, 32) * 2, mod(blockPositionOut.y, 32) * 2, mod(blockPositionOut.z, 32) * 2);

    // Normalize tint values to be in the range 0 - 1
    tint = ((tint + vec3(1.0)) / 255.0 * 10);

    vec4 finalColor = baseColor * vec4(tint, 1.0);

    FragColor = finalColor;
}