#ifndef PARTICLE_H
#define PARTICLE_H

// 粒子类

class Particle {
public:

    // 构造函数

    Particle(float x, float y, float z, float vx, float vy, float vz, float life, float r, float g, float b, float a);
    
    // 更新粒子状态


    void update(float deltaTime);
    
    // 检查粒子是否存活


    bool isAlive() const;
    
public:

    // 位置

    float x, y, z;
    
    // 速度

    float vx, vy, vz;
    
    // 生命周期

    float life;
    float maxLife;
    
    // 颜色

    float r, g, b, a;
    
    // 大小

    float size;
    
    // 重力加速度

    float gravity = 50.0f; // 增加重力加速度以适应3D空间
};

#endif // PARTICLE_H
