#version 430 core

in  vec2 vTex;
in  vec3 vWorldPos;
out vec4 FragColor;

// 注意：字段名改用非保留字（如 isActive），但内存布局必须与 C++ PointLight 一致
struct PointLight {
    vec3 position;   // offset 0, padded to 16B
    vec3 color;      // offset 16, padded to 16B
    float constant;  // offset 32
    float linear;    // offset 36
    float quadratic; // offset 40
    float isActive;  // offset 44 (对应 C++ 的 active 字段)
    // total: 48 bytes — 完全匹配 C++ 结构体
};

layout(std140, binding = 0) uniform LightBlock {
    PointLight lights[20];
};

uniform sampler2D groundTex;
uniform vec3 baseColor = vec3(1.0);
uniform int debugMode = 2;

const vec3 GROUND_NORMAL = vec3(0.0, 1.0, 0.0);

vec3 computeLighting() {
    vec3 result = vec3(0.0);
    for (int i = 0; i < 20; ++i) {
        if (lights[i].isActive == 0.0) continue;  // 使用 isActive
        vec3 L = lights[i].position - vWorldPos;
        float dist = length(L);
        float atten = 1.0 / (
            lights[i].constant +
            lights[i].linear * dist +
            lights[i].quadratic * dist * dist
        );
        float diff = max(0.0, dot(GROUND_NORMAL, normalize(L)));
        result += lights[i].color * diff * atten* 3.0;
    }
    return result;
}

void main() {
    vec3 albedo = texture(groundTex, vTex).rgb * baseColor;

    if (debugMode == 0) {
        FragColor = vec4(albedo, 1.0);
        return;
    }

    if (debugMode == 1) {
        bool anyLight = false;
        for (int i = 0; i < 20; ++i) {
            if (lights[i].isActive != 0.0) {  // 使用 isActive
                anyLight = true;
                break;
            }
        }
        FragColor = vec4(anyLight ? 1.0 : 0.0, 0.0, 0.0, 1.0);
        return;
    }

    // debugMode == 2
    vec3 lighting = computeLighting();
    FragColor = vec4(albedo * (0.5 + lighting), 1.0);
}