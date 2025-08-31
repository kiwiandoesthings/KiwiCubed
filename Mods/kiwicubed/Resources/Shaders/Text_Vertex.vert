#version 330 core

layout (location = 0) in vec4 vertex;

out vec2 textureCoordinatesOut;

uniform mat4 modelMatrix;

void main()
{
    gl_Position = modelMatrix * vec4(vertex.x, vertex.y, 0, 1);
    //gl_Position = vertex;
    textureCoordinatesOut = vertex.zw;
}