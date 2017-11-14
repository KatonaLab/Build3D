#version 150 core

noperspective in vec2 screenCoordGeom;
in vec3 tex3DCoordGeom;

out vec4 outputColor;

uniform sampler2D backFaceMap;
uniform sampler3D volumeTexture;
uniform vec4 volumeColor;
uniform vec4 lutParameters;
uniform float accumDivisor;
uniform float visible;

float lut(in sampler3D tex, in vec3 pos, in vec4 params)
{
    // normalize to [0, 1] by dividing with max data value
    float x = texture(tex, pos).r / params.y;
    float a = params.z;
    float b = params.w;
    float r = max(b - a, 0.001); // prevent division by zero, TODO: do something with it, it doesnt work
    // return 0 if x < a
    // return x if a < x < b
    // return 0 if b < x
    // https://www.wolframalpha.com/input/?i=((min(max(x,+2.5),+4)+-+2.5)%2F(4-2.5)+-+step(x-4)
    return (clamp(x, a, b) - a) / r - step(b, x);
}

void main()
{
    vec3 far = texture(backFaceMap, screenCoordGeom.xy).xyz;
    vec3 near = tex3DCoordGeom;
    float alpha = 0.;
    for (int i = 0; i <= 32; ++i) {
        vec3 pos = mix(far, near, float(i) * 1./32.);
        alpha = alpha + lut(volumeTexture, pos, lutParameters);
    }
    outputColor = vec4(accumDivisor * visible * alpha * volumeColor.rgb, 1.0);
}
