#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "firework.h"
#include "shader.h"
#include <vector>
#include <random>
#include <glad/glad.h>
#include <glm/glm.hpp>


// 粒子系统类

class ParticleSystem {

public:

    // 构造函数

    ParticleSystem(int maxFireworks);
    
    // 初始化

    void init();
    
    // 更新

    void update(float deltaTime);
    
    // 渲染

    void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    
    // 添加烟花

    void addFirework();
    
private:

    // 烟花列表

    std::vector<Firework> fireworks;
    
    // 最大烟花数量

    int maxFireworks;
    
    // 随机数生成器

    std::random_device rd;
    std::mt19937 gen;
    
    // 生成随机数

    float random(float min, float max);
    int randomInt(int min, int max);
    
    // 着色器

    Shader* particleShader;
    
    // VAO和VBO

    unsigned int VAO, VBO;
    
    // 粒子数据

    std::vector<float> particleData;
    
    // 最大粒子数量

    const int MAX_PARTICLES = 10000;
};

#endif // PARTICLE_SYSTEM_H
