#version 150 core

in vec3 vertexPosition;
in vec3 tex3DCoords;
out vec3 position;
uniform mat4 modelViewProjection;

void main()
{
    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);
    position = tex3DCoords;
}