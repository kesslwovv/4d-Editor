#version 330

out vec4 outColor;

in float w_pos;
in vec4 vColor;

uniform vec4 color;
uniform float intensity = 1;
uniform float bleed_size = 1;
uniform bool useTex=false;
uniform sampler3D mainTex;


#define E 2.71828182845904523

vec4 fragColor()
{
    if (useTex) {
        return texture(mainTex, vColor.xyz / vColor.w);
    } else {
        return vec4(color.xyz * vColor.xyz, 1);
    }
}

vec4 getColor()
{
    float det = 1 / log(abs(w_pos/bleed_size)+E);
    float visibility = det*intensity;
    vec4 col = fragColor();
    col.a = color.a * visibility;
    col.xyz *= col.a;
//    col.a = 1;
    return col;
}

void main()
{
    outColor = getColor();
    
//    outColor.a = min(outColor.a, .3);
    // max(1 - abs(w_pos * 0.5), 0)
}

