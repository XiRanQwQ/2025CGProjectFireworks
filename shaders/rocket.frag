#version 330 core

in vec4 v_color; // 来自顶点着色器的颜色信息
in float v_lifeT; // 粒子的生命时间比例，用于控制拖尾效果等
out vec4 FragColor;

uniform sampler2D u_trailMask; // 拖尾纹理
uniform bool u_useTrail = false; // 控制是否使用拖尾效果

void main(){
    vec2 coord = gl_PointCoord - vec2(0.5); // 将坐标中心移到(0, 0)
    float dist = length(coord); // 计算到中心的距离
    
    // 超出圆域直接扔掉
    if (dist > 0.5) discard;
    
    // 基础alpha值，根据距离中心的距离平滑变化
    float alpha = smoothstep(0.5, 0.1, dist);
    
    // 如果启用了拖尾效果
    if(u_useTrail)
    {
        // 使用拖尾纹理的alpha通道来调节整体alpha值
        // 注意：这里假设拖尾纹理是按照粒子坐标映射的，可能需要适当调整uv坐标
        vec2 uv = gl_PointCoord; // 或者根据实际需求计算uv坐标
        float trailAlpha = texture(u_trailMask, uv).a;
        alpha *= trailAlpha; // 结合基础alpha与拖尾alpha
    }
    
    // 设置最终的颜色输出，应用alpha混合
    FragColor = vec4(v_color.rgb, v_color.a * alpha);
}