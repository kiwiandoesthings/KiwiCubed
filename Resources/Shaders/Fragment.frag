#version 330 core

out vec4 FragColor;

in vec2 texCoord;
in vec3 blockPos;

uniform sampler2D tex0;

void main()
{
	vec4 baseColor = texture(tex0, texCoord);

	//vec3 tint = vec3(blockPos.x, blockPos.y, blockPos.z);

    // Normalize tint values to be in the range [0, 1]
    //tint = ((tint + vec3(1.0)) / 255.0 * 10);

    // Apply tint to the base color
    vec4 finalColor = baseColor;// * vec4(tint, 1.0);

    // Output final color
    FragColor = finalColor;
}