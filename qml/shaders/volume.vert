#version 150 core

in vec3 vertexPosition;
in vec3 tex3DCoords;
// important not to do perspective correct interpolation, 
// because it is in the screen space
noperspective out vec4 screenCoord;
out vec3 position;
uniform mat4 modelViewProjection;

void main()
{
    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);
    screenCoord = gl_Position / gl_Position.w * 0.5 + 0.5;
    position = tex3DCoords;
}