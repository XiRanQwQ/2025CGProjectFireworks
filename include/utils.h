#ifndef UTILS_H
#define UTILS_H

#include<glm/glm.hpp>

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

#endif // UTILS_H