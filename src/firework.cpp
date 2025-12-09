#include "firework.h"
#include <cmath>
#define M_PI 3.14159265358979323846

Firework::Firework(float x, float y, float z, float vx, float vy, float vz, int particleCount, float r, float g, float b)
    : rocket(x, y, z, vx, vy, vz, 5.0f, r, g, b, 1.0f),
    status(LAUNCHING),
    particleCount(particleCount),
    r(r), g(g), b(b)
{
    // 初始化随机数生成器


}

void Firework::update(float deltaTime) {
    switch (status) {
    case LAUNCHING:

        // 更新火箭粒子

        rocket.update(deltaTime);


        // 检查是否到达最高点（速度接近零）

        if (rocket.isAlive() && rocket.vy < 0.1f) {
            explode();
            status = EXPLOSING;
        }
        else if (!rocket.isAlive()) {
            status = FINISHED;
        }
        break;

    case EXPLOSING:

        // 更新爆炸产生的粒子


        for (auto it = particles.begin(); it != particles.end();) {
            it->update(deltaTime);
            if (!it->isAlive()) {
                it = particles.erase(it);
            }
            else {
                ++it;
            }
        }


        // 检查是否所有粒子都消失了


        if (particles.empty()) {
            status = FINISHED;
        }
        break;

    case FINISHED:
        break;
    }
}

// 渲染函数现在由ParticleSystem类统一处理，这里不再实现

bool Firework::isFinished() const {
    return status == FINISHED;
}

// 获取烟花当前状态

FireworkStatus Firework::getStatus() const {
    return status;
}

// 获取火箭粒子

const Particle& Firework::getRocket() const {
    return rocket;
}

// 获取爆炸粒子列表

const std::vector<Particle>& Firework::getParticles() const {
    return particles;
}

void Firework::explode() {

    // 创建爆炸粒子

    for (int i = 0; i < particleCount; ++i) {

        // 随机速度（圆周方向）

        float angle = random(0.0f, 2.0f * M_PI);
        float speed = random(50.0f, 200.0f);
        float vx = cos(angle) * speed;
        float vy = sin(angle) * speed;


        // 随机生命周期

        float life = random(1.0f, 2.0f);


        // 随机颜色变化

        float particleR = random(r - 0.2f, r + 0.2f);
        float particleG = random(g - 0.2f, g + 0.2f);
        float particleB = random(b - 0.2f, b + 0.2f);


        // 确保颜色在有效范围内

        particleR = particleR < 0.0f ? 0.0f : (particleR > 1.0f ? 1.0f : particleR);
        particleG = particleG < 0.0f ? 0.0f : (particleG > 1.0f ? 1.0f : particleG);
        particleB = particleB < 0.0f ? 0.0f : (particleB > 1.0f ? 1.0f : particleB);


        // 添加粒子


        particles.emplace_back(rocket.x, rocket.y, rocket.z, vx, vy, 0.0f, life, particleR, particleG, particleB, 1.0f);
    }
}

float Firework::random(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}