#version 330

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;


//uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec4 offset = vec4(0, 0, 0, 0);

uniform vec4 position = vec4(0, 0, 0, 0);
uniform mat4 transform = mat4(1);


out float w_pos;
out vec4 vColor;


void main()
{
    vec4 pos =  view * (aPos * transform + position - offset);
    pos.z += 0.001;
    gl_Position = proj * vec4(pos.xyz, 1);
    w_pos = pos.w;
    vColor = aColor;
//    gl_Position = proj * view * vec4(aPos.xyz, 1);
}


