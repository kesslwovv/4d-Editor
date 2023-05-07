#version 330

out vec2 coord;

vec2 verts[6] = vec2[6](
    vec2(1, 1),
    vec2(1, -1),
    vec2(-1, 1),
                        
    vec2(1, -1),
    vec2(-1, 1),
    vec2(-1, -1)
);

void main()
{
    int idx = gl_VertexID;
//    if (idx > 2) {
//        idx -= 2;
//    }
    vec2 vt = verts[idx] * 2;
    coord = vt;
    gl_Position = vec4(vt, 0.5, 1);
}
