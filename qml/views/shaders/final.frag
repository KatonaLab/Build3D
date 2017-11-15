#version 150 core

noperspective in vec2 screenCoord;
out vec4 outputColor;

uniform sampler2D accumulatorMap;

void main()
{    
    outputColor = texture(accumulatorMap, screenCoord);
}