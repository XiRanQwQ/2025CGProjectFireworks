#include "particle.h"
#include "utils.h"   // 里面定义了 Phys::dragFactor / Phys::gravity
#include<iostream>

Particle::Particle(float x, float y, float z, float vx, float vy, float vz, float life, float r, float g, float b, float a) {
	this->position = glm::vec3(x, y, z);
    this->velocity = glm::vec3(vx, vy, vz);
	this->life = life;
	this->maxLife = life;
	this->color = glm::vec4(r, g, b, a);
	this->size = 4.0f;
  

  

}

Particle::Particle(glm::vec3 pos, glm::vec3 vel, float life, glm::vec4 color )
    :position(pos),velocity(vel),life(life),maxLife(life),color(color) {
	this->size = 4.0f;
}

Particle::Particle(glm::vec3 pos, glm::vec3 vel, float life, glm::vec4 color, bool isTrail, bool isPhysicsEnabled)
	: position(pos), velocity(vel), life(life), maxLife(life), color(color),size(4.0f),isTrail(isTrail), isPhysicsEnabled(isPhysicsEnabled)
{
}





void Particle::update(float dt, float sway)
{
    life -= dt ;
    if (isPhysicsEnabled) {
       velocity.y -= gravity * dt;
        velocity -= air_drag * glm::length(velocity) * velocity * dt;
    } 
    position += velocity * dt;
}

bool Particle::isAlive() const {

    return life > 0.0f;

}
