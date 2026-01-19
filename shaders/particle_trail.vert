#version 330 core
layout(location = 0) in vec3 aPos;          // 0-2  当前坐标
layout(location = 1) in vec4 aColor;        // 3-6  出生主色 + 初始 alpha
layout(location = 2) in float aLife;        // 7    剩余 life
layout(location = 3) in float aMaxLife;     // 8    总寿命
layout(location = 4) in float aSize;        // 9    初始大小

out vec4  v_color;   // vec4 输出（rgb + 内部 alpha）
out float v_lifeT;
out float v_pointSize; // 新增：实际像素直径

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
    float lifeT = aLife / aMaxLife;

    // 1. 直接输出原色，不做任何白炽
    v_color   = vec4(aColor.rgb, aColor.a * lifeT);
    v_lifeT   = lifeT;

    // 2. 先小后大 + 透视缩放（保留）
    vec4 viewPos   = view * model * vec4(aPos, 1.0);
    float depth    = max(-viewPos.z, 1.0);
    float expandT  = 1.0 - lifeT;
    float startSize= aSize * 0.2f;
    float endSize  = aSize * 2.0f;
    v_pointSize    = mix(startSize, endSize, expandT);

    gl_Position    = projection * viewPos;
    gl_PointSize   = v_pointSize * (500.0 / depth);
}