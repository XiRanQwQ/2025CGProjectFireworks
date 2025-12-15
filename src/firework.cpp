#include "firework.h"
#include <cmath>
#include <iostream>
#include "utils.h"
#include<GLFW/glfw3.h>

Firework::Firework(float x, float y, float z, float vx, float vy, float vz, int particleCount, float r, float g, float b, float a)
    : rocket(x, y, z, vx, vy, vz, vy * 1.1f / gravity, r, g, b, a),
	particleCount(particleCount), status(LAUNCHING)
{
   
}

Firework::Firework(glm::vec3 position, glm::vec3 velocity, int particleCount, glm::vec4 color,FireworkType type)
    : rocket(position, velocity, velocity.y*1.1f/gravity, color),
	particleCount(particleCount), status(LAUNCHING),  type(type)
{

    
    
}




void Firework::update(float dt)
{

    if (status == LAUNCHING ) {
        if(!rocket.isTrail){
            particles.emplace_back(rocket.position, glm::vec3{0.f},
                random(0.3f, 1.0f), rocket.color,true,true);
		}
    }

    // 爆炸拖影：每帧一个点（主粒子已更新完）
    if (status == EXPLOSING) {
        for (size_t i = 0; i < particles.size(); ++i) {
            if (!particles[i].isTrail) {
                if (type == common) {
                    particles.emplace_back(particles[i].position, glm::vec3{0.f},
                        random(0.5f, 1.5f), particles[i].color,true,true);
                }
                else if(type==scatter){
                    particles.emplace_back(particles[i].position, particles[i].velocity,
                        random(1.8f, 2.4f), particles[i].color, true, true);
                }else if(type==star){
                    particles.emplace_back(particles[i].position, particles[i].velocity,
                        random(1.8f, 2.4f), particles[i].color, true, false);
				}

                
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
        rocket.update(dt,Firework::sway);
        if (rocket.isAlive() && rocket.velocity.y < 0.0f) {
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
    if(type==common){
        explode_common();
    }
    else if(type==scatter) {
		explode_scatter();
    }
    else if (type == star) {
        explode_star();
    }
    else if (type == flower) {
        explode_flower();
    }
    
}

void Firework::explode_common()
{
    int innerCount = static_cast<int>(particleCount * 0.4f);
    int outerCount = particleCount - innerCount;

    // 内球参数
    float innerSpeed = random(10.f, 30.f);
    float innerLife = random(0.5f, 1.2f);

    // 外球参数
    float outerSpeed = random(30.f, 100.f);
    float outerLife = random(1.5f, 2.5f);


    auto color1 = glm::vec4(random_vec3(), 1.0f);
    auto color2 = glm::vec4(random_vec3(), 1.0f);

    glm::vec3 dir;
    // 内球
    for (int i = 0; i < innerCount; ++i) {
        dir = sphereRand();
        dir.x = 0.f;
        float speed = innerSpeed * random(0.9f, 1.1f);
        glm::vec3 vel = dir * speed;

        glm::vec3 c = glm::mix(color1, color2, random(0.f, 0.3f));
        particles.emplace_back(rocket.position, vel, innerLife, glm::vec4(c, 1.f));
        
    }

    // 外球
    for (int i = 0; i < outerCount; ++i) {
		dir = sphereRand();
        dir.x = 0.f;
        float speed = outerSpeed * random(0.9f, 1.1f);
        glm::vec3 vel = dir * speed;
        
        glm::vec3 c = glm::mix(color2, color1, random(0.f, 0.2f));
        particles.emplace_back(rocket.position, vel, outerLife, glm::vec4(c, 1.f));
       
    }
}

void Firework::explode_scatter(){
    float baseSpeed = random(30.f, 100.f);
    float life = random(0.5f, 1.5f);
    /* 散点版：单球+单一随机颜色+每粒子独立速度扰动 */
    glm::vec4 singleColor = glm::vec4(random_vec3(), 1.0f); // 一种随机色
    for (int i = 0; i < particleCount; ++i) {
        glm::vec3 dir = sphereRand();
        float   speed = baseSpeed * random(0.8f, 1.2f); // 每粒子独立速度
        glm::vec3 vel = dir * speed;
        particles.emplace_back(rocket.position, vel, life, singleColor);
    } 
}

void Firework::explode_star()
{
    const float outerR = 2.5f;
    const float innerR = 1.0f;
    const float speed = random(6.f, 6.f);
    const float life = random(10.5f, 10.5f);
    glm::vec4 color = glm::vec4(random_vec3(), 1.0f);

    float angle = random(0.f,2*M_PI );
    auto data = starRayPoints(30, outerR, innerR, speed, life, rocket.position,angle);

    for (size_t i = 0; i < data.size(); i += 3) {   // 每 3 个 vec3 是一份粒子
        Particle p(data[i], data[i + 1], data[i + 2].x, color);
        p.size = random(2.f, 5.f);
        p.isTrail = true;
        p.isPhysicsEnabled = false;
        particles.push_back(p);
    }
}

void Firework::explode_flower()
{
    const float outerR = 2.5f;
    const float innerR = 1.0f;
    const float speed = random(6.f, 6.f);
    const float life = random(10.5f, 10.5f);
    glm::vec4 color = glm::vec4(random_vec3(), 1.0f);

    float angle = random(0.f, 2 * M_PI);
    
    auto data = flowerRayPoints(30, outerR, innerR, speed, life, rocket.position, angle);

    for (size_t i = 0; i < data.size(); i += 3) {   // 每 3 个 vec3 是一份粒子
        std::cout<<"Flower Particle No."<<(i/3)+1<<": velocity=("<<data[i+1].x<<","<<data[i+1].y<<","<<data[i+1].z<<")"<<std::endl;
        Particle p(data[i], data[i + 1], data[i + 2].x, color);
        p.size = random(2.f, 5.f);
        p.isTrail = true;
        p.isPhysicsEnabled = false;
        particles.push_back(p);
    }
}

