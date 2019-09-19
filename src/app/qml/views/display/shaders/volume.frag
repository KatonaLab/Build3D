#version 150 core

in vec4 screenCoordGeom;
in vec3 tex3DCoordGeom;

out vec4 outputColor;

uniform sampler2D backFaceMap;
uniform sampler3D volumeTexture;
uniform vec4 volumeColor;
uniform vec4 lutParameters; // dataMinValue=0, dataMaxValue=1, lutLowCut, lutHightCut
uniform float accumDivisor;
uniform float visible;
uniform float labeled;

//----------------------------------------------------------------------------------------
// Credits to David Hoskins 
//----------------------------------------------------------------------------------------

// Hash without Sine
// Creative Commons Attribution-ShareAlike 4.0 International Public License
// Created by David Hoskins.

// https://www.shadertoy.com/view/4djSRW
// Trying to find a Hash function that is the same on ALL systens
// and doesn't rely on trigonometry functions that change accuracy 
// depending on GPU. 
// New one on the left, sine function on the right.
// It appears to be the same speed, but I suppose that depends.

// * Note. It still goes wrong eventually!
// * Try full-screen paused to see details.


// *** Change these to suit your range of random numbers..

// *** Use this for integer stepped ranges, ie Value-Noise/Perlin noise functions.
#define HASHSCALE1 .1031
#define HASHSCALE3 vec3(.1031, .1030, .0973)
#define HASHSCALE4 vec4(.1031, .1030, .0973, .1099)

// For smaller input rangers like audio tick or 0-1 UVs use these...
// #define HASHSCALE1 443.8975
// #define HASHSCALE3 vec3(443.897, 441.423, 437.195)
// #define HASHSCALE4 vec3(443.897, 441.423, 437.195, 444.129)

vec3 hash31(float p)
{
   vec3 p3 = fract(vec3(p) * HASHSCALE3);
   p3 += dot(p3, p3.yzx+19.19);
   return fract((p3.xxy+p3.yzz)*p3.zyx); 
}

//----------------------------------------------------------------------------------------

float lut(in float t, in vec4 params)
{
    // normalize to [0, 1] by dividing with max data value
    float x = (t - params.x) / params.y;
    float a = params.z - 0.0001;
    float b = params.w + 0.0001; // small offset to prevent div by 0
    float r = b - a;
    // return (x - a)/(b - a) if a <= x <= b
    // return 0 otherwise
    // https://www.wolframalpha.com/input/?i=((min(max(x,+2.5),+4)+-+2.5)%2F(4-2.5)+-+step(x-4)
    return (clamp(x, a, b) - a) / r - step(b, x);
}

vec3 getLabelColor(in sampler3D tex, in vec3 pos)
{
    float x = texture(tex, pos).r;
    return hash31(x);
}

void main()
{
    vec2 uv = (screenCoordGeom.xyz / screenCoordGeom.w * 0.5 + 0.5).xy;
    vec3 far = texture(backFaceMap, uv).xyz;
    vec3 near = tex3DCoordGeom;
    float alpha = 0.;
    vec3 labelColor = vec3(0.);
    for (int i = 0; i <= 48; ++i) {
        vec3 pos = mix(far, near, float(i) * 1./48.);
        float x = texture(volumeTexture, pos).r;
        alpha = max(alpha, lut(x, lutParameters));
        // if (labeled < 0.5) {
        //     continue;
        // }
        // labelColor = labelColor + getLabelColor(volumeTexture, pos) * 1./48.;
    }
    vec3 finalColor = mix(alpha * volumeColor.rgb, labelColor, labeled);
    outputColor = vec4(accumDivisor * visible * finalColor, 1.0);
    // outputColor = vec4(step(0.5, vec3(uv.x, uv.y, 0.0)), 1.0);
    // outputColor = vec4(far, 1.0);
}
