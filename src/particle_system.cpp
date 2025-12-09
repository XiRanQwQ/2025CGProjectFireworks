#include "particle_system.h"
#include <GLFW/glfw3.h>
#include <iostream>

ParticleSystem::ParticleSystem(int maxFireworks)
    : maxFireworks(maxFireworks),
    gen(rd()) {
}

void ParticleSystem::init() {
    // 加载粒子着色器

    particleShader = new Shader("D:/CGFinalProject/CGFP/shaders/particle.vert", "D:/CGFinalProject/CGFP/shaders/particle.frag");

    // 初始化粒子数据向量

    particleData.reserve(MAX_PARTICLES * (3 + 4)); // 位置(3) + 颜色(4)

    // 创建VAO和VBO

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 绑定VAO

    glBindVertexArray(VAO);

    // 绑定VBO

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * (3 + 4) * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // 设置位置属性

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 + 4) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 设置颜色属性

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (3 + 4) * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 解绑VAO

    glBindVertexArray(0);

    // 启用混合

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystem::update(float deltaTime) {
    // 随机添加烟花（增加概率，便于测试）

    if (fireworks.size() < maxFireworks && random(0.0f, 1.0f) < 0.3f) {
        addFirework();
    }

    // 更新所有烟花

    for (auto it = fireworks.begin(); it != fireworks.end();) {
        it->update(deltaTime);
        if (it->isFinished()) {
            it = fireworks.erase(it);
        }
        else {
            ++it;
        }
    }
}

void ParticleSystem::render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    // 收集所有粒子数据
    particleData.clear();

    for (auto& firework : fireworks) {
        // 添加火箭粒子数据（如果正在发射阶段）

        if (firework.getStatus() == LAUNCHING) {
            const Particle& rocket = firework.getRocket();
            particleData.push_back(rocket.x);
            particleData.push_back(rocket.y);
            particleData.push_back(rocket.z);
            particleData.push_back(rocket.r);
            particleData.push_back(rocket.g);
            particleData.push_back(rocket.b);
            particleData.push_back(rocket.a);
        }

        // 添加爆炸粒子数据

        for (const Particle& particle : firework.getParticles()) {
            particleData.push_back(particle.x);
            particleData.push_back(particle.y);
            particleData.push_back(particle.z);
            particleData.push_back(particle.r);
            particleData.push_back(particle.g);
            particleData.push_back(particle.b);
            particleData.push_back(particle.a);
        }
    }

    // 如果没有粒子，直接返回

    if (particleData.empty()) {
        return;
    }

    // 使用粒子着色器

    particleShader->use();

    // 设置投影和视图矩阵

    particleShader->setMat4("projection", projectionMatrix);
    particleShader->setMat4("view", viewMatrix);
    particleShader->setMat4("model", glm::mat4(1.0f));

    // 更新VBO数据

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particleData.size() * sizeof(float), particleData.data());

    // 绑定VAO

    glBindVertexArray(VAO);

    // 渲染所有粒子

    glDrawArrays(GL_POINTS, 0, particleData.size() / (3 + 4));

    // 解绑VAO

    glBindVertexArray(0);
}

void ParticleSystem::addFirework() {
    // 随机位置

    float x = random(-300.0f, 300.0f);
    float y = -300.0f; // 从底部发射

    float z = random(-300.0f, 300.0f);

    // 随机速度

    float vx = random(-50.0f, 50.0f);
    float vy = random(300.0f, 500.0f);
    float vz = random(-50.0f, 50.0f);

    // 粒子数量

    int particleCount = randomInt(50, 200);

    // 随机颜色

    float r = random(0.0f, 1.0f);
    float g = random(0.0f, 1.0f);
    float b = random(0.0f, 1.0f);

    // 创建烟花

    fireworks.emplace_back(x, y, z, vx, vy, vz, particleCount, r, g, b);
}

float ParticleSystem::random(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

int ParticleSystem::randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}