#version 330 core
in  vec4  v_color;
in  float v_lifeT;
out vec4 FragColor;

void main(){
    vec2  coord = gl_PointCoord - vec2(0.5);
    float dist  = length(coord);
    if (dist > 0.5) discard;           // ≥¨≥ˆ‘≤”Ú÷±Ω”»”µÙ
    float alpha = smoothstep(0.5, 0.1, dist);
    FragColor   = vec4(v_color.rgb, v_color.a * alpha);
}