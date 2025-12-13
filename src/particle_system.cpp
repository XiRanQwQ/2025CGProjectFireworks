#include "particle_system.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <stb_image.h>

ParticleSystem::ParticleSystem(int maxFireworks)
    : maxFireworks(maxFireworks),
    gen(rd()) {
}

void ParticleSystem::init() {
    // 启用程序控制的点大小

    glEnable(GL_PROGRAM_POINT_SIZE);  

    // 加载粒子着色器

<<<<<<< HEAD
    // 使用相对路径，确保可执行工作目录包含 shaders 目录
    particleShader = new Shader("C:/Users/lenovo/Desktop/2025CGProjectFireworks/shaders/particle.vert", "C:/Users/lenovo/Desktop/2025CGProjectFireworks/shaders/particle.frag");
=======
    particleShader = new Shader("shaders/particle.vert", "shaders/particle.frag");
>>>>>>> a84869da9409fdb65200a8a4124e90fa54e25bbd

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

    // 使顶点着色器中设置的 gl_PointSize 生效
    glEnable(GL_PROGRAM_POINT_SIZE);
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

void ParticleSystem::addParticleForRendering(const Particle& particle) {
    particleData.push_back(particle.position.x);
    particleData.push_back(particle.position.y);
    particleData.push_back(particle.position.z);
    particleData.push_back(particle.color.r);
    particleData.push_back(particle.color.g);
    particleData.push_back(particle.color.b);
    particleData.push_back(particle.color.a);
}

void ParticleSystem::addTrailStrip(const Particle& particle) {
    const glm::vec3* h = particle.hist;
    int head = particle.histHead;

    // 从老→新遍历，形成 POINTS 序列
    for (int i = 0; i < Particle::SNAP; ++i) {
        int idx = (head + i) % Particle::SNAP;
        glm::vec3 pos = h[idx];

        float age = float(Particle::SNAP - 1 - i) / float(Particle::SNAP - 1);   // 0→1

        /* 以烟花主色为“热端”，暗橙红为“冷端” */
        
        glm::vec3 hot = glm::vec3(particle.color.r, particle.color.g, particle.color.b); // 主色（最新）                                
        glm::vec3 cool = glm::mix(hot, glm::vec3(0.8f, 0.3f, 0.05f), 0.7f);   // 暗橙红（最老）
        cool *= 0.4f;                                                           // 再压暗
        glm::vec3 rgb = glm::mix(hot, cool, age);                              // 新→老：主色→暗冷

        float alpha = (1.0f - age) * 0.6f;
        float sizeFactor = (1.0f - age) * 0.9f + 0.1f;
        glm::vec4 col = glm::vec4(rgb, sizeFactor);   // rgb=温度，a=大小因子

        addParticleForRendering(Particle(pos, glm::vec3(0.0f), 0.1f, col));
    }
}

void ParticleSystem::render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    /*---------- 1. 收集所有顶点数据（拖尾 + 粒子） ----------*/
    particleData.clear();

    /* 1.1 拖尾：每枚火箭 6 个历史点 → 连续光球带 */
    for (auto& fw : fireworks) {
        if (fw.getStatus() == LAUNCHING) {
            addTrailStrip(fw.getRocket());   // 推 6 顶点，带 age→alpha/size
        }
    }
    int trailVerts = particleData.size() / 7;   // 记录拖尾顶点数

    /* 1.2 粒子：火箭 + 爆炸碎片 */
    for (auto& fw : fireworks) {
        if (fw.getStatus() == LAUNCHING)
            addParticleForRendering(fw.getRocket());
        for (const auto& p : fw.getParticles())
            addParticleForRendering(p);
    }
    int particleVerts = particleData.size() / 7 - trailVerts; // 纯粒子顶点数

    /*---------- 2. 统一上传 GPU ----------*/
    if (!particleData.empty()) {
        particleShader->use();
        particleShader->setMat4("projection", projectionMatrix);
        particleShader->setMat4("view", viewMatrix);
        particleShader->setMat4("model", glm::mat4(1.0f));

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, particleData.size() * sizeof(float), particleData.data());
        glBindVertexArray(VAO);

        /* 3.1 拖尾：每点 1 画，大小/alpha 随 age 梯度变化 */
        for (int i = 0; i < trailVerts; ++i)
            glDrawArrays(GL_POINTS, i, 1);   // 1 点 1 画，大小=aColor.a

        /* 3.2 粒子：一次性画所有点 */
        if (particleVerts > 0)
            glDrawArrays(GL_POINTS, trailVerts, particleVerts);

        glBindVertexArray(0);
    }
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