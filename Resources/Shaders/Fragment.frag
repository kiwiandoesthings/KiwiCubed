#version 330 core

out vec4 FragColor;

in vec2 textureCoordinateOut;

uniform sampler2D tex0;

void main()
{
	vec4 baseColor = texture(tex0, textureCoordinateOut);
    FragColor = baseColor;
}

//#version 330 core
//out vec4 FragColor;
//void main() {
//    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
//}