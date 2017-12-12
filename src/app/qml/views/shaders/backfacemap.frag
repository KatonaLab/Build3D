#version 150 core

in vec3 position;
out vec4 outputColor;

void main()
{    
    outputColor = vec4(position, 1.0);
}