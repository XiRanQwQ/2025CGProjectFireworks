#ifndef UTILS_H
#define UTILS_H

#include<glm/glm.hpp>
#include<iostream>
inline constexpr float M_PI = 3.14159265358979323846f;

inline constexpr float gravity = 9.81f;

inline constexpr float air_drag = 0.031f;

// 返回 [min, max] 区间的均匀随机浮点数

float random(float min, float max);

// 返回 [min, max] 区间的均匀随机整数（包含两端）

int random(int min, int max);

// 返回 0~1 之间的随机浮点数

inline float random01() { return random(0.0f, 1.0f); }


// 返回每个分量在 [0,1] 之间的随机 vec3
inline glm::vec3 random_vec3(){
	return glm::vec3(random01(), random01(), random01());
}

// 均匀球面随机单位向量（各向同性）
inline glm::vec3 sphereRand() {
    // Marsaglia 方法：拒绝采样，避免三角函数
    float x, y, z;
    do {
        x = random(-1.0f, 1.0f);
        y = random(-1.0f, 1.0f);
        z = random(-1.0f, 1.0f);
    } while (x * x + y * y + z * z > 1.0f);   // 拒绝超球面点

    return glm::normalize(glm::vec3(x, y, z));
}

// 均匀球面随机 + 可调半径
inline glm::vec3 sphereRand(float radius) {
    return sphereRand() * radius;
}

// 均匀球体随机点（含体积）
inline glm::vec3 randomInSphere(float radius) {
    glm::vec3 point;
    do {
        point = glm::vec3(random(-radius, radius), random(-radius, radius), random(-radius, radius));
    } while (glm::length(point) > radius);
    return point;
}

inline glm::vec4 hsv2rgb(float h, float s, float v){
    float c = v * s;
    float x = c * (1 - std::fabs(std::fmod(h / 60.0f, 2) - 1));
    float m = v - c;
    float r = 0, g = 0, b = 0;
    if (h < 60) { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }
    return glm::vec4(r + m, g + m, b + m, 1.0f);
};


// 输入：角度 ang（弧度），输出：该方向上的星形边界半径
inline float starRadius(float ang, float R_outer = 1.0f, float R_inner = 0.4f)
{
    float phi = fmod(ang - M_PI/2.0f, 2.0f * M_PI / 5.0f); // 映射到 [0, 72°)
    if (phi < 0) phi += 2.0f * M_PI / 5.0f;

    if (phi <= M_PI / 5.0f) // 0~36°
        return R_outer - (R_outer - R_inner) * phi * 5.0f / M_PI;
    else                    // 36~72°
        return R_inner + (R_outer - R_inner) * (phi - M_PI / 5.0f) * 5.0f / M_PI;
}


inline std::vector<glm::vec3>
flowerRayPoints(int countPerLobe,
    float R_outer,
    float R_inner,
    float baseSpeed,
    float life,
    const glm::vec3& center,
    float BaseRad)          // 整体旋转（弧度）
{
    std::vector<glm::vec3> out;
    int total = countPerLobe * 10;                // 5 外顶 + 5 内顶 ≈ 10 段
    float dAng = float(2 * M_PI) / float(total);  // 等角步长

    out.reserve(total * 3);                       // 每粒子 3 个 vec3

    for (int i = 0; i < total; ++i) {
        float ang = i * dAng + BaseRad;           // ① 先整体旋转
        float r = starRadius(ang, R_outer, R_inner); // ② 极坐标边界（保留弧线）

        // 位置：X=0 平面
        glm::vec3 local(0.f, r * std::cos(ang), r * std::sin(ang));
        glm::vec3 pos = center + local;

        // 速度：径向向外，大小 ∝ 边界半径（保留弧形速度场）
        glm::vec3 radial(0.f, std::cos(ang), std::sin(ang));
        glm::vec3 vel = radial * (r * baseSpeed);

        // 打包：pos, vel, life
        out.push_back(pos);
        out.push_back(vel);
        out.push_back(glm::vec3(life));
    }
    return out;
}

inline void linePoints(const glm::vec2& A,
    const glm::vec2& B,
    int steps,
    float baseSpeed,
    float life,
    const glm::vec3& center,
    std::vector<glm::vec3>& out)
{
    for (int i = 0; i < steps; ++i) {
        float t = (i + 0.5f) / steps;                 // 中心采样
        glm::vec2 yz = glm::mix(A, B, t);            // 直线插值
        glm::vec3 pos(center.x, center.y + yz.x, center.z + yz.y);

        // 速度：径向向外（Y-Z 平面）
        glm::vec3 radial(0.f, yz.x, yz.y);
        if (float len = glm::length(radial); len > 0.f)
            radial /= len;
        glm::vec3 vel = radial * (glm::length(yz) * baseSpeed);

        out.push_back(pos);
        out.push_back(vel);
        out.push_back(glm::vec3(life));
    }
}

inline std::vector<glm::vec3>
starRayPoints(int pointsPerEdge,
    float R_outer,
    float R_inner,
    float baseSpeed,
    float life,
    const glm::vec3& center,
    float baseRad)// 整体旋转量
{
    std::vector<glm::vec3> out;
    out.reserve(pointsPerEdge * 10 * 3);

    

    // 1. 预计算 10 个顶点（先旋转）
    glm::vec2 verts[10];
    for (int i = 0; i < 10; ++i) {
        float ang = glm::radians(i * 36.f - 90.f);
        float r = (i & 1) ? R_inner : R_outer;
        float x = r * cos(ang);
        float z = r * sin(ang);
        // 2-D 旋转
        verts[i].x = x * cos(baseRad) - z * sin(baseRad);
        verts[i].y = x * sin(baseRad) + z * cos(baseRad);
    }

    // 2. 10 条直线段，复用 linePoints
    for (int edge = 0; edge < 10; ++edge) {
        linePoints(verts[edge], verts[(edge + 1) % 10],
            pointsPerEdge, baseSpeed, life, center, out);
    }
    return out;
}






#endif // UTILS_H