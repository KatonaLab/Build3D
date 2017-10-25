#version 150 core

in vec3 position;
// important not to do perspective correct interpolation, 
// because it is in the screen space
noperspective in vec4 screenCoord;
out vec4 outputColor;
uniform sampler2D backfaceMap;

uniform sampler3D ch0texture;
uniform sampler3D ch1texture;
uniform sampler3D ch2texture;
uniform sampler3D ch3texture;

uniform vec4 ch0color;
uniform vec4 ch1color;
uniform vec4 ch2color;
uniform vec4 ch3color;

uniform vec4 ch0cutParams;
uniform vec4 ch1cutParams;
uniform vec4 ch2cutParams;
uniform vec4 ch3cutParams;

void main()
{
    vec3 far = texture(backfaceMap, screenCoord.xy).xyz;
    vec3 near = position;
    vec4 acc = vec4(0.);
    for (int i = 0; i < 32; ++i) {
        vec3 pos = mix(far, near, float(i) * 0.0625*0.5); // 0.0625 = 1/16
        acc += texture(ch0texture, pos);
        acc += texture(ch1texture, pos);
        acc += texture(ch2texture, pos);
        acc += texture(ch3texture, pos);
    }
    outputColor = vec4(acc.rgb * 0.01, acc.r * 0.1);
}