#version 330


layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in gl_PerVertex
{
    vec4 gl_Position;
} gl_in[];

in float wPos[];
in vec4 transformed[];
in vec4 vertexColor[];

out float wPosFrag;
out vec2 pos;
out vec4 vColor;

uniform mat4 proj;
uniform float point_size;

#define POINT_MUL 0.1

void main()
{
//    gl_Position = gl_in[0].gl_Position + vec4(POINT_MUL * point_size, POINT_MUL * point_size, 0, 0);
    gl_Position = proj * vec4(transformed[0].xyz + vec3(POINT_MUL * point_size, POINT_MUL * point_size, 0), 1);
    pos = vec2(1, 1);
    wPosFrag = wPos[0];
    vColor = vertexColor[0];
    EmitVertex();
//    gl_Position = gl_in[0].gl_Position + vec4(-POINT_MUL * point_size, POINT_MUL * point_size, 0, 0);
    gl_Position = proj * vec4(transformed[0].xyz + vec3(-POINT_MUL * point_size, POINT_MUL * point_size, 0), 1);
    pos = vec2(-1, 1);
    wPosFrag = wPos[0];
    vColor = vertexColor[0];
    EmitVertex();
//    gl_Position = gl_in[0].gl_Position + vec4(POINT_MUL * point_size, -POINT_MUL * point_size, 0, 0);
    gl_Position = proj * vec4(transformed[0].xyz + vec3(POINT_MUL * point_size, -POINT_MUL * point_size, 0), 1);
    pos = vec2(1, -1);
    wPosFrag = wPos[0];
    vColor = vertexColor[0];
    EmitVertex();
//    gl_Position = gl_in[0].gl_Position + vec4(-POINT_MUL * point_size, -POINT_MUL * point_size, 0, 0);
    gl_Position = proj * vec4(transformed[0].xyz + vec3(-POINT_MUL * point_size, -POINT_MUL * point_size, 0), 1);
    pos = vec2(-1, -1);
    wPosFrag = wPos[0];
    vColor = vertexColor[0];
    EmitVertex();
    EndPrimitive();
}
