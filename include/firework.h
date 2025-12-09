#ifndef FIREWORK_H
#define FIREWORK_H

#include "particle.h"
#include <vector>
#include <random>

// 烟花状态枚举


enum FireworkStatus {

    LAUNCHING,    
    EXPLOSING,    
    FINISHED      
};

// 烟花类

class Firework {
public:

    // 构造函数

    Firework(float x, float y, float z, float vx, float vy, float vz, int particleCount, float r, float g, float b);
    
    // 更新烟花状态

    void update(float deltaTime);
    
    // 渲染烟花

    void render();
    
    // 检查烟花是否已结束

    bool isFinished() const;
    
    // 获取烟花当前状态

    FireworkStatus getStatus() const;
    
    // 获取火箭粒子

    const Particle& getRocket() const;
    
    // 获取爆炸粒子列表

    const std::vector<Particle>& getParticles() const;
    
private:

    // 烟花粒子

    Particle rocket;
    
    // 爆炸产生的粒子

    std::vector<Particle> particles;
    
    // 状态

    FireworkStatus status;
    
    // 粒子数量

    int particleCount;
    
    // 颜色

    float r, g, b;
    
    // 随机数生成器

    //std::random_device rd;

    std::mt19937 gen;
    
    // 爆炸函数

    void explode();
    
    // 生成随机数

    float random(float min, float max);
};

#endif // FIREWORK_H
