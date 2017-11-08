#version 150 core

noperspective in vec2 screenCoordGeom;
in vec3 tex3DCoordGeom;

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

float lut(in sampler3D tex, in vec3 pos, in vec4 params)
{
    // normalize to [0, 1] by dividing with max data value
    float x = texture(tex, pos).r / params.y;
    float a = params.z;
    float b = params.w;
    float r = b - a;
    // return 0 if x < a
    // return x if a < x < b
    // return 0 if b < x
    // https://www.wolframalpha.com/input/?i=((min(max(x,+2.5),+4)+-+2.5)%2F(4-2.5)+-+step(x-4)
    return (clamp(x, a, b) - a) / r - step(b, x);
}

void main()
{
    vec3 far = texture(backfaceMap, screenCoordGeom.xy).xyz;
    vec3 near = tex3DCoordGeom;
    vec4 alpha = vec4(0.);
    float step = 0.03;
    float t = 0.0;
    for (int i = 0; i <= 32; ++i) {
        // vec3 pos = mix(far, near, float(i) * 1./32.);
        vec3 pos = near + normalize(far - near) * t;
        t += step;

        alpha.r = (alpha.r + lut(ch0texture, pos, ch0cutParams));
        alpha.g = (alpha.g + lut(ch1texture, pos, ch1cutParams));
        alpha.b = (alpha.b + lut(ch2texture, pos, ch2cutParams));
        alpha.a = (alpha.a + lut(ch3texture, pos, ch3cutParams));
    }
    outputColor = (alpha.r * ch0color * ch0color.a
        + alpha.g * ch1color * ch1color.a
        + alpha.b * ch2color * ch2color.a
        + alpha.a * ch3color * ch3color.a);

    outputColor = vec4(distance(near, far) * 0.5, 0., 0., 1.);
}
