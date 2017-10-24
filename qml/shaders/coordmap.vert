#version 150 core

in vec3 vertexPosition;
in vec3 tex3DCoords;

out vec3 position;
// out vec3 direction;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjection;
uniform vec3 eyePosition;

void main()
{
    // position.xyz = vertexPosition.xyz + 0.5;
    // vec4 d = modelMatrix * vec4(vertexPosition, 1.0) - vec4(eyePosition, 1.);
    // direction = d.xyz;
    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);
    position = tex3DCoords;
}