#version 330


out vec4 outColor;

in vec2 coord;

uniform sampler2D ColorA;
uniform sampler2D ColorB;
uniform sampler2D DepthA;
uniform sampler2D DepthB;


vec2 texPos;

vec4 getColor()
{
    vec4 colorA  = texture(ColorA, texPos);
    vec4 colorB  = texture(ColorB, texPos);
    float depthA = texture(DepthA, texPos).x;
    float depthB = texture(DepthB, texPos).x;
//    return vec4(depthA * 0.8, depthB * 0.8, colorA.r, 1);
    

//    colorB /= colorB.a + 0.1;
    if (colorB.a > 1) {
        colorB.xyz /= colorB.a;
    }
//    colorA /= colorA.a;
//    colorA /= 1;
    
//    if (false && depthA <= depthB) {
//        return colorA;
//    } else {
//        return mix(colorB, colorA, 1-colorB.a);
//    }
    return mix(colorA, colorB, .5);
//    return vec4(1, 0, 0, 1);
}

void main()
{
    texPos = coord * .5 + .5;
    
    vec4 col = getColor();
    col.a = 1;
    outColor = col;
}
