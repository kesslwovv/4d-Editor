#version 330


out vec4 outColor;

in float w_pos;
in vec4 vColor;

uniform vec4 color;
uniform float intensity = 1;
uniform float bleed_size = 1;
uniform bool useTex=false;
uniform sampler3D mainTex;

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
    float det = exp(-w_pos*w_pos/bleed_size);
    vec4 origCol = fragColor();
    return vec4(origCol.xyz, det*intensity);
    /*
    float n = abs(w_pos) * 5;
    vec3 col = clamp(vec3(4 - n, min(n - 1, 6 - n), max(min(n, 3 - n), min(n - 4, 7 - n))), 0, 1);
    col -= 0.5;
    col *= max(1 - (n / 10), 0.2);
    col += 0.5;
    return col;
     */
}

void main()
{
    vec4 col = getColor();
//    if (col.w < 0.01) {
//        discard;
//    }
    outColor = col;
}
