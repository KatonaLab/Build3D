#version 150 core

in vec3 vertexPosition;

out vec3 position;

uniform mat4 modelViewProjection;
uniform mat4 vertexToTex3DCoordMatrix;

void main()
{
    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);
    position = (vertexToTex3DCoordMatrix * vec4(vertexPosition, 1.0)).xyz;
}