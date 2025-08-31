#version 330 core

in vec2 textureCoordinatesOut;

out vec4 FragColor;

uniform sampler2D tex0;
uniform vec3 textColor;

void main() {
    float alpha = texture(tex0, textureCoordinatesOut).r;
    FragColor = vec4(textColor, alpha);
    //FragColor = vec4(255, 255, 0, 1);
}
