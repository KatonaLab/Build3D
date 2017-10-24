#version 150 core

in vec3 position;
in vec3 direction;

out vec4 outputColor;

// uniform sampler3D teximage;
// uniform float time;
// uniform float threshold;
// uniform float thresholding;

void main()
{
    // vec3 d = direction;
    // // vec3 d = direction;
    // float acc = 0.0;
    // float t = 0.;
    // for (int i = 0; i < 256; ++i) {
    //     vec3 p = position + d * float(i)*0.015;
    //     acc = acc + step(0.05, texture(teximage, p).r);
    //     // 0.25 * 
    // }
    // outputColor = vec4(vec3(acc*0.05*0.5), 1.);
    // // outputColor = vec4(2.*texture(teximage, position).rrr, 1.);
    
    outputColor = vec4(position, 1.0);
}