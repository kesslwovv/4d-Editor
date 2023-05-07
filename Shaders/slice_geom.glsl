#version 330

#define LIGHT 1

layout(lines_adjacency) in;
layout(triangle_strip, max_vertices=4) out;

in gl_PerVertex
{
    vec4  gl_Position;
} gl_in[];

in TRANSFORMED_POSITION
{
    vec4 pos;
    vec4 orig;
    vec4 color;
} vert[];

uniform mat4 proj;

//out vec4 pos;
//out vec4 origPos;
//out vec4 normal;
out vec4 fragColor;

float light;

//#define LIGHT_DIR vec4(0.5, -1, -0.15, -0.2)
uniform vec4 lightDir = vec4(0, 1, 0, 0);

uniform bool useLight=true;

void handleVertex(int idx1, int idx2)
{
    
    vec4 pos1 = vert[idx1].pos;
    vec4 pos2 = vert[idx2].pos;
    if (sign(pos1.w) == sign(pos2.w)) {
        return;
    }
    vec4 color1 = vert[idx1].color;
    vec4 color2 = vert[idx2].color;
    
    float mix_var = abs(pos1.w) / (abs(pos1.w) + abs(pos2.w));
    
    vec4 pos = mix(pos1, pos2, mix_var);
    fragColor = mix(color1, color2, mix_var);
    fragColor.xyz *= light;
//    origPos = mix(vert[idx1].orig, vert[idx2].orig, mix_var);
//    normal = norm;
    gl_Position = proj * vec4(pos.xyz, 1);

    EmitVertex();

}

vec4 perp(vec4 a, vec4 b, vec4 c, vec4 d)
{
    a -= d;
    b -= d;
    c -= d;
    
    float x = determinant(mat4(a, b, c, vec4(1, 0, 0, 0)));
    float y = determinant(mat4(a, b, c, vec4(0, 1, 0, 0)));
    float z = determinant(mat4(a, b, c, vec4(0, 0, 1, 0)));
    float w = determinant(mat4(a, b, c, vec4(0, 0, 0, 1)));
    
    return normalize(vec4(x, y, z, w));
}


void main()
{
    {
        float sgn = sign(vert[0].pos.w);
        if (sgn == sign(vert[1].pos.w)
            && sgn == sign(vert[2].pos.w)
            && sgn == sign(vert[3].pos.w)) {
            return;
        }
    }
    
//#if LIGHT
//    vec4 norm = perp(vert[0].orig, vert[1].orig, vert[2].orig, vert[3].orig);
//    light = max(abs(dot(norm, normalize(lightDir))), 0.3) + .2;
//    // TODO: remove abs when normals are fixed
//#else
//    light = 1;
//#endif
    

    if (useLight) {
        vec4 norm = perp(vert[0].orig, vert[1].orig, vert[2].orig, vert[3].orig);
        light = max(abs(dot(norm, normalize(lightDir))), 0.3) + .2;
        // TODO: remove abs when normals are fixed
    } else {
        light = 1;
    }
    
    handleVertex(0, 1);
    handleVertex(0, 2);
    handleVertex(0, 3);
    handleVertex(1, 2);
    handleVertex(1, 3);
    handleVertex(2, 3);
    
    EndPrimitive();
}
