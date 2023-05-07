
#version 330

/*
in vec2 pos;
in float wPosFrag;

in vec4 vColor;

in vec4 gl_FragCoord;
*/

in float wPos;

in float pointSize;

in vec4 vertexColor;
in vec4 transformed;

out vec4 outColor;
//out gl_FragDepth;

uniform vec4 color;
uniform float bleed_size;
uniform float color_bleed;

vec4 getColor()
{
    vec4 col = min(vertexColor, 1);
    col.a = exp(-wPos*wPos / bleed_size);
    if (pointSize < 1) {
        col.a *= pointSize;
    }
    return col;
//    return vColor;
//    float normalInput = wPosFrag/bleed_size/color_bleed;
//    float det = exp(-normalInput*normalInput);
//    return mix(vec4(0.5, 0.5, 0.5, 1), vec4(color.xyz, 1), det);
}

void main()
{
    vec2 relpos = gl_PointCoord * 2 - 1;
    if (dot(relpos, relpos) > 1 && pointSize > 1.01) {
        discard;
    }
//    float sqdist = dot(pos, pos);
//    if (sqdist > 1-abs(wPosFrag)/bleed_size) {
//        discard;
//    }
//    gl_FragDepth = gl_FragCoord.z - sqrt(1 - sqdist) * 0.0001;
    outColor = getColor();
}

