#version 150 core

noperspective in vec4 screenCoord;
in vec3 position;
out vec4 outputColor;
uniform sampler2D backfaceMap;

void main()
{
    vec4 t = texture(backfaceMap, screenCoord.xy);
    // outputColor = mix(vec4(position, 1.0), vec4(t.xyz, 1.0), 0.5);
    // outputColor = vec4(screenCoord.rg, 0., 1.0);
    outputColor = vec4(vec3(distance(position, t.xyz)), 1.0);
}