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

//烟花类型枚举（可重构优化）
enum FireworkType {
	common,     //普通烟花
	scatter,    //散点型烟花
	star,        //星形烟花
    flower,
};

// 烟花类

class Firework {
public:

    inline static const float sway = 0.5f;

    // 构造函数
    Firework(float x, float y, float z, float vx, float vy, float vz, int particleCount, float r, float g, float b,float a);
    Firework(glm::vec3 position, glm::vec3 velocity, int particleCount,glm::vec4 color,FireworkType type=common);
	
    

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


	FireworkType type;

    // 爆炸函数

    void explode();
    
	void explode_common();
    void explode_scatter();
    void explode_star();
	void explode_flower();
};



#endif // FIREWORK_H
