#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 windowViewMatrix;


void main() {
    vec3 toCamera = normalize(cameraPos - position);

    mat3 rotationMatrix = mat3(
        normalize(vec3(1.0, 0.0, 0.0)),
        normalize(vec3(0.0, 1.0, 0.0)),
        normalize(toCamera)
    );

    vec3 rotatedPos = rotationMatrix * position;

    gl_Position = projection * view * vec4(rotatedPos, 1.0);
}

// SEPARATE OUT WINDOWVIEWMATRIX IN ALL SHADERS, FINISH CHUNK REBUG RENDER, LOOK IN MAIN FOR REST OF TODO