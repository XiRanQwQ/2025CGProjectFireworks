#version 330 core
layout(location = 0) in vec3 aPos;          // 0-2  当前坐标
layout(location = 1) in vec4 aColor;        // 3-6  出生主色 + 初始 alpha
layout(location = 2) in float aLife;        // 7    剩余 life
layout(location = 3) in float aMaxLife;     // 8    总寿命
layout(location = 4) in float aSize;        // 9    初始大小

out vec4  v_color;   // vec4 输出（rgb + 内部 alpha）
out float v_lifeT;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    gl_PointSize = aSize; 
    v_color = aColor;
    v_lifeT = aLife / aMaxLife;
}
