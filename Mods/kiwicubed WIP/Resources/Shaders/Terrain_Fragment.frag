#version 330 core

out vec4 FragColor;

in vec3 blockPositionOut;
in vec2 textureCoordinateOut;
in float blockTypeOut;

uniform sampler2D tex0;
uniform uint atlasSize;

const float epsilon = 0.00001;


void main() {
    //vec3 tint = vec3(mod(blockPositionOut.x, 32) * 2, mod(blockPositionOut.y, 32) * 2, mod(blockPositionOut.z, 32) * 2);
    //tint = ((tint + vec3(1.0)) / 255.0 * 10);
    vec3 tint = vec3(1.0, 1.0, 1.0);

    float textureIndexX = mod(blockTypeOut - 1.0 + epsilon, float(atlasSize));
    float textureIndexY = floor((blockTypeOut - 1.0 + epsilon) / float(atlasSize));

    FragColor = texture(tex0, vec2(
        (textureCoordinateOut.x / float(atlasSize)) + (textureIndexX / float(atlasSize)),
        (textureCoordinateOut.y / float(atlasSize)) + (textureIndexY / float(atlasSize))
    )) * vec4(tint, 1.0);
}
