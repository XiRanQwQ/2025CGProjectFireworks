#include "firework.h"
#include <cmath>
#include <iostream>
#include "utils.h"
#include<GLFW/glfw3.h>

Firework::Firework(float x, float y, float z, float vx, float vy, float vz, int particleCount, float r, float g, float b, float a)
    : rocket(x, y, z, vx, vy, vz, 2.0f, r, g, b, a),
	particleCount(particleCount), status(LAUNCHING)
{
    color1 = glm::vec4(random_vec3(), 1.0f);
	color2 = glm::vec4(random_vec3(), 1.0f);
}

Firework::Firework(glm::vec3 position, glm::vec3 velocity, int particleCount, glm::vec4 color)
    : rocket(position, velocity, velocity.y*1.1f/gravity, color),
    particleCount(particleCount), status(LAUNCHING)
{

    color1 = glm::vec4(random_vec3(), 1.0f);
    color2 = glm::vec4(random_vec3(), 1.0f);
}

Firework::Firework(glm::vec3 position, glm::vec3 velocity, int particleCount, glm::vec4 color, glm::vec4 color1, glm::vec4 color2)
    : rocket(position, velocity, velocity.y * 1.1f / gravity, color),
	particleCount(particleCount), status(LAUNCHING), color1(color1), color2(color2)
{

}


void Firework::update(float dt)
{
    // ===== 1. 统一生火焰拖影点 =====
    static float lastRocketTrail = 0.0f;
    float        now = float(glfwGetTime());
	//std::cout << "dt" << now - lastRocketTrail << std::endl;
    // 火箭拖影：每 0.05 s 一个点
    if (status == LAUNCHING  /*&&now - lastRocketTrail > 0.002f&&glm::length(rocket.velocity*(now-lastRocketTrail))>2.0f*/) {
        lastRocketTrail = now;
        Particle trail(rocket.position, rocket.velocity,
            random(0.3f,1.f), rocket.color);
        trail.color.a = 1.0f;
        trail.isTrail = true;
        particles.push_back(trail);
    }

    // 爆炸拖影：每帧一个点（主粒子已更新完）
    if (status == EXPLOSING) {
        for (size_t i = 0; i < particles.size(); ++i) {
            if (!particles[i].isTrail) {
                particles.emplace_back(particles[i].position, particles[i].velocity,
                    random(1.8f, 2.4f), particles[i].color);
                particles.back().isTrail = true;
            }
        }
    }

    // ===== 2. 统一更新 & 回收 =====
    for (auto it = particles.begin(); it != particles.end();) {
        it->update(dt);
        if (!it->isAlive()) {
            it = particles.erase(it);
        }
        else {
            ++it;
        }
    }

    // ===== 3. 爆炸检测 =====
    switch (status) {
    case LAUNCHING:
        rocket.update(dt);
        if (rocket.isAlive() && rocket.velocity.y < 0.1f) {
            std::cout << "Firework explode at position: "
                << rocket.position.x << ", "
                << rocket.position.y << ", "
                << rocket.position.z << std::endl;
            explode();
            status = EXPLOSING;
        }
        else if (!rocket.isAlive()) {
            status = FINISHED;
        }
        break;
    case EXPLOSING:
        if (particles.empty()) status = FINISHED;
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

void Firework::explode()
{
    int innerCount = static_cast<int>(particleCount * 0.4f);
    int outerCount = particleCount - innerCount;

    // 内球参数
    float innerSpeed = random(50.f, 140.f);
    float innerLife = random(0.5f, 1.2f);

    // 外球参数
    float outerSpeed = random(120.f, 220.f);
    float outerLife = random(0.5f, 1.5f);

    // ****** 关键：球面随机 → 压平 X → 圆盘在 Y-Z 平面
    auto diskRand = [](glm::vec3& dir) {
        dir = sphereRand();          // 各向同性球面
        dir.x = 0.0f;                // 压平 X → 圆盘
        dir = glm::normalize(dir);   // 重新归一化
        };

    // 内球
    for (int i = 0; i < innerCount; ++i) {
        glm::vec3 dir;
        diskRand(dir);
        float speed = innerSpeed * random(0.9f, 1.1f);
        glm::vec3 vel = dir * speed;

        glm::vec3 c = glm::mix(color1, color2, random(0.f, 0.3f));
        particles.emplace_back(rocket.position, vel, innerLife, glm::vec4(c, 1.f));
    }

    // 外球
    for (int i = 0; i < outerCount; ++i) {
        glm::vec3 dir;
        diskRand(dir);
        float speed = outerSpeed * random(0.9f, 1.1f);
        glm::vec3 vel = dir * speed;

        glm::vec3 c = glm::mix(color2, color1, random(0.f, 0.2f));
        particles.emplace_back(rocket.position, vel, outerLife, glm::vec4(c, 1.f));
    }
}

