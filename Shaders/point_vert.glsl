#version 330

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;

uniform mat4 proj;
uniform mat4 view;
uniform vec4 offset = vec4(0, 0, 0, 0);

uniform vec4 position = vec4(0, 0, 0, 0);
uniform mat4 transform = mat4(1);

uniform float sizeMul = 5;
uniform float maxSize = 30;

uniform bool useTex=false;
uniform sampler3D mainTex;

out float wPos;

out float pointSize;

out vec4 vertexColor;
out vec4 transformed;


vec4 getColor()
{
    if (useTex) {
        return texture(mainTex, aColor.xyz);
    } else {
        return vec4(aColor.xyz, 1);
    }
}

void main()
{
    vec4 pos =  view * (aPos * transform + position - offset);

    transformed = pos;
    vertexColor = getColor();
    
    pointSize = 1 / -pos.z * sizeMul;
    gl_PointSize = clamp((pointSize), 1, maxSize);

    gl_Position = proj * vec4(pos.xyz, 1);
    wPos = pos.w;
}


