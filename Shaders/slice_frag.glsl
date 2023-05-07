#version 330

out vec4 outColor;

in vec4 gl_FragCoord;

//in vec4 pos;
//in vec4 normal;
//in vec4 fragColor;
in vec4 fragColor;

uniform vec4 color;

uniform bool useTex=false;
uniform sampler3D mainTex;


vec4 frCol()
{
    if (useTex) {
        return texture(mainTex, fragColor.xyz / fragColor.w);
    } else {
        return vec4(color.xyz * fragColor.xyz, fragColor.w);
    }
}


#define LIGHT_DIR vec4(0.1, -1, -0.15, -0.2)

vec3 getColor()
{
    vec4 fCol = frCol();
    return (fCol.xyz / fCol.w) * color.xyz;

    
//    float light = abs(dot(normal, LIGHT_DIR)) + 0.01;
//
//    float distanceModifier = exp(pos.z * 0.05);
////    float dimlight = max(light, 0.2);
////    dimlight += light * 0.1;
////
////    dimlight += gl_FragCoord.z * 0.1;
////
////    dimlight = min(dimlight, 0.9);
//
//    return light * distanceModifier * color.xyz * fragColor.xyz;
}

void main()
{
//    if (abs(pos.w) > 0.00001) {
//        discard;
//    }

    outColor = vec4(getColor(), 1);
}
