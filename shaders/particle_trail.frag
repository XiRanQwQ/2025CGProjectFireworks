#version 330 core
in  vec4  v_color;
in  float v_lifeT;
in  float v_pointSize;   // 实际像素直径
out vec4 FragColor;

uniform sampler2D u_gradient;   // 保留，暂不用

void main(){
    // 1. 中心亮-边缘暗（径向衰减）
    float dist = length(gl_PointCoord - 0.5) * 2.0;        // 0→1
    float radial = 1.0 - dist;                             // 中心=1 边缘=0
    radial = pow(radial, 3.0);                             // 可调幂次

    // 2. 生命初期「过曝抑制」曲线
    // lifeT: 1.0(出生) → 0.0(死亡)
    // 前 20 % 时间亮度被额外压低，避免中心叠白
    float suppress = smoothstep(0.0, 0.5, 1.0 - v_lifeT);  // 0→1
    float brightness = radial * (0.01 + 0.99 * suppress);    // 0.7→1.0

    // 3. 最终颜色（alpha 仍由顶点衰减）
    vec3 rgb = v_color.rgb * brightness;
    FragColor = vec4(rgb, v_color.a);
}