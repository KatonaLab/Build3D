#version 150 core

in vec3 vertexPosition; // TODO: consider using 'invariant' qualifier

void main()
{
    // NOTE: bypass for gl_Position,
    // coord transformation will take place at geom shader
    gl_Position = vec4(vertexPosition, 1.0);
}