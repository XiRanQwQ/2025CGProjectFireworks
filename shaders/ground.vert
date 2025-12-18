#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;

out vec2 vTex;
out vec3 vWorldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    vec4 world = model * vec4(aPos, 1.0);
    vWorldPos  = world.xyz;
    vTex       = aTex;
    gl_Position= projection * view * world;
}