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

// 廉价伪随机
uint hash1(uint x){ x ^= x >> 16; x *= 0x7feb352dU; x ^= x >> 15; return x; }
float hash1f(uint x){ return float(hash1(x)) / float(0xffffffffU); }

vec3 fireColor(float t){
    t = clamp(t, 0.0, 1.0);
    vec3 w = vec3(1.0, 0.95, 0.8);   // 白炽
    vec3 r = vec3(1.0, 0.15, 0.02);  // 深红
    return mix(w, r, t * t);
}

void main(){
    float lifeT = aLife / aMaxLife;               // 0→1 生命比
    float temp  = 0.8 + 0.2 * hash1f(uint(gl_VertexID));
    float spark = 0.85 + 0.15 * hash1f(uint(gl_VertexID + 1));

    // 主色调：粒子自身颜色 ↔ 火焰色 混合
    vec3 baseRgb = aColor.rgb;
    vec3 flameRgb = fireColor(lifeT * temp);
    vec3 rgb = mix(baseRgb, flameRgb, 0.5) * spark; // 0.5 可调

    v_color   = vec4(rgb, aColor.a * lifeT); // alpha 衰减
    v_lifeT   = lifeT;

    gl_Position  = projection * view * model * vec4(aPos, 1.0);
    gl_PointSize = aSize * lifeT * 2;  
}