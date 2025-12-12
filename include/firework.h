#ifndef FIREWORK_H
#define FIREWORK_H

#include "particle.h"
#include <vector>


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
    Firework(float x, float y, float z, float vx, float vy, float vz, int particleCount, float r, float g, float b,float a);
    Firework(glm::vec3 position, glm::vec3 velocity, int particleCount,glm::vec4 color);
	Firework(glm::vec3 position, glm::vec3 velocity, int particleCount, glm::vec4 color, glm::vec4 color1, glm::vec4 color2);
    
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

    // 火箭粒子

    Particle rocket;
    
    // 爆炸产生粒子和拖影点精灵

    std::vector<Particle> particles;

    
    // 状态

    FireworkStatus status;
    
    // 粒子数量

    int particleCount;
    
    // 爆炸颜色

	glm::vec3 color1; //爆炸内圈颜色

	glm::vec3 color2; //爆炸外圈颜色

    int sparkCount = 10;

    // 爆炸函数

    void explode();
    
    

};

#endif // FIREWORK_H
