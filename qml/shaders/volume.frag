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

float cut(in sampler3D tex, in vec3 pos, in vec4 params)
{
    // normalize to [0, 1] by dividing with max data value
    float x = texture(tex, pos).r / params.y;
    float a = params.z;
    float b = params.w;
    return ((clamp(x, a, b) - a) - (b-a) * step(b, x)) / b;
}

void main()
{
    vec3 far = texture(backfaceMap, screenCoord.xy).xyz;
    vec3 near = position;
    vec4 alpha = vec4(0.);
    for (int i = 0; i <= 16; ++i) {
        vec3 pos = mix(far, near, float(i) * 1./16.);
        alpha.r += cut(ch0texture, pos, ch0cutParams);
        alpha.g += cut(ch1texture, pos, ch1cutParams);
        alpha.b += cut(ch2texture, pos, ch2cutParams);
        alpha.a += cut(ch3texture, pos, ch3cutParams);
    }
    outputColor = (alpha.r * ch0color * ch0color.a
        + alpha.g * ch1color * ch1color.a
        + alpha.b * ch2color * ch2color.a
        + alpha.a * ch3color * ch3color.a);
}