#version 330

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;

uniform mat4 proj;
uniform mat4 view;
uniform vec4 offset = vec4(0, 0, 0, 0);

uniform vec4 position = vec4(0, 0, 0, 0);
uniform mat4 transform = mat4(1);

out TRANSFORMED_POSITION
{
    vec4 pos;
    vec4 orig;
    vec4 color;
} trans;


void main()
{
    vec4 pos =  view * (aPos * transform + position - offset);
    trans.pos = pos;
    trans.orig = aPos - position;
    trans.color = vec4(aColor.xyz, 1) * aColor.w;

    gl_Position = proj * vec4(pos.xyz, 1);
}

