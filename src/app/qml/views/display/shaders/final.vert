#version 150 core

in vec3 vertexPosition;
noperspective out vec2 screenCoord;

void main()
{
    gl_Position = vec4(vertexPosition.xz, 0.0, 1.0);
    screenCoord = vertexPosition.xz * 0.5 + 0.5;
}