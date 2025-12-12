#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in float aLife;
layout(location = 3) in float aMaxLife;
layout(location = 4) in float aSize;

out vec4 v_color;
out float v_lifeT;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    v_lifeT = aLife / aMaxLife;

    if (v_lifeT <= 0.0) {
        gl_Position = vec4(0.0);
        gl_PointSize = 0.0;
        return;
    }

    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // 根据你的设定：顶点在 v_lifeT ≈ 0.91
    const float APEX_LIFE_T = 0.91;

    vec3 baseColor = aColor.rgb;
    vec3 whiteHot  = vec3(1.0, 1.0, 1.0);

    vec3 finalColor;
    float alpha = aColor.a;
    float pointSize = aSize ;

    if (v_lifeT >= APEX_LIFE_T) {
        // === 上升阶段：接近顶点时逐渐变白 ===
        // lifeT: 1.0 → 0.91，映射到 whiteness: 0.0 → 1.0
        float t = (1.0 - v_lifeT) / (1.0 - APEX_LIFE_T); // 0 → 1
        t = clamp(t, 0.0, 1.0);
        finalColor = mix(baseColor, whiteHot, t);
        // 保持亮度和大小
    } else {
        // === 下降阶段（爆炸后）：从白色快速衰减到透明，不再变回红色！===
        float fade = v_lifeT / APEX_LIFE_T; // 0.91 → 0 映射到 1 → 0
        fade = clamp(fade, 0.0, 1.0);
        finalColor = whiteHot * fade; // 直接从白色衰减！
        alpha *= fade;
        pointSize *= fade;
    }

    v_color = vec4(finalColor, alpha);
    gl_PointSize = pointSize;
}