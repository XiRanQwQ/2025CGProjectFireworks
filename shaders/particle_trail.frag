#version 330 core
in  vec4  v_color;
in  float v_lifeT;
in  float v_pointSize;   // 实际像素直径
out vec4 FragColor;

uniform sampler2D u_gradient;   // 1×64 梯度纹理

void main(){
    // 1. 梯度遮罩
    float dist = length(gl_PointCoord - 0.5) * 2.0;
    float g    = texture(u_gradient, vec2(dist, 0.5)).r;
    if(g < 0.01) discard;          // 可选边缘剔除

    // 2. 中心过曝：按「像素半径」压暗
    float pixelDist = dist * v_pointSize;              // 0 → v_pointSize
    float core = 1.0 - smoothstep(0.0, v_pointSize * 0.6, pixelDist);
    core = pow(core, 4.0);                             // 可调幂次

    // 3. 边缘淡出：按像素距离
    float edge = 1.0 - smoothstep(v_pointSize * 0.7, v_pointSize, pixelDist);

    // 4. 最终颜色
    vec3 rgb = v_color.rgb * core * edge;
    FragColor = vec4(rgb, 1.0);
}