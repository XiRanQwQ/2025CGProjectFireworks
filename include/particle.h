#ifndef PARTICLE_H
#define PARTICLE_H

#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
// 粒子类

class Particle {
public:

    // 构造函数

    Particle(float x, float y, float z, float vx, float vy, float vz, float life, float r, float g, float b, float a);
	Particle(glm::vec3 pos, glm::vec3 vel, float life, glm::vec4 color);
    
    // 更新粒子状态


    void update(float deltaTime);
    
    // 检查粒子是否存活


    bool isAlive() const;
    
public:

    // 位置

    glm::vec3 position;
    
    // 速度

	glm::vec3 velocity;
    
    // 生命周期

    float life;
    float maxLife;
    
    // 颜色

	glm::vec4 color;
    
    // 大小

    float size;
    
    // 重力加速度
    static constexpr float GRAVITY = 250.0f;   // 直接给值

    
    static constexpr int SNAP = 60;              // 拖尾缓冲数组长度
    glm::vec3 hist[SNAP] = {};                  // 历史位置
    int   histHead = 0;                         // 写指针
    float histTimer = 0.0f;                     // 累计定时器
	float histInterval = 0.02f;                 // 采样间隔时间
};

#endif // PARTICLE_H
