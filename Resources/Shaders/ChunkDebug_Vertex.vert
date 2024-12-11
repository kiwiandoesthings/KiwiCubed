#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 chunkOrigin;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform vec3 cameraPosition;


void main() {
    vec3 right = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
    vec3 up = vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

    vec3 worldPosition = chunkOrigin + right * position.x + up * position.y;

    gl_Position = projectionMatrix * viewMatrix * vec4(worldPosition, 1.0);
}