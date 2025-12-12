#include "particle.h"

Particle::Particle(float x, float y, float z, float vx, float vy, float vz, float life, float r, float g, float b, float a) {
	this->position = glm::vec3(x, y, z);
    this->velocity = glm::vec3(vx, vy, vz);
	this->life = life;
	this->maxLife = life;
	this->color = glm::vec4(r, g, b, a);
	this->size = 4.0f;
  

  

}

Particle::Particle(glm::vec3 pos, glm::vec3 vel, float life, glm::vec4 color ):position(pos),
velocity(vel),life(life),maxLife(life),color(color) {
	this->size = 4.0f;
}



// particle.cpp
#include "utils.h"   // 里面定义了 Phys::dragFactor / Phys::gravity

void Particle::update(float dt)
{
    if (isTrail) {              // 尾迹粒子：只做淡出，不动位置
        life -= dt;
        color.a = glm::clamp(life / maxLife, 0.f, 1.f)*0.5 ; 
        return;                  // 直接退出，跳过后续物理
    }

    // 重力、阻力、运动
    velocity.y -= gravity * dt;
    velocity -= air_drag * glm::length(velocity) * velocity * dt;
    position += velocity * dt;
    life -= dt;
}

bool Particle::isAlive() const {

    return life > 0.0f;

}
