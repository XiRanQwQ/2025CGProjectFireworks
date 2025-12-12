#version 330 core
in  vec4  v_color;
in  float v_lifeT;
out vec4 FragColor;

uniform sampler2D u_gradient;   // 1×64 径向高斯

void main(){
    float dist = length(gl_PointCoord - 0.5) * 2.0;
    float g    = texture(u_gradient, vec2(dist, 0.5)).r;
    if(g < 0.01) discard;          // 可选：裁掉边缘

    float fade = v_color.a * v_lifeT * g;   // 0→1 衰减
    vec3  rgb  = v_color.rgb * fade;        // 关键：RGB 也降下去

    FragColor = vec4(rgb, 1.0);             // 加性混合不需要 alpha<1
}