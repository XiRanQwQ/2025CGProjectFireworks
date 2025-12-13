#include "particle.h"

Particle::Particle(float x, float y, float z, float vx, float vy, float vz, float life, float r, float g, float b, float a) {
	this->position = glm::vec3(x, y, z);
    this->velocity = glm::vec3(vx, vy, vz);
	this->life = life;
	this->maxLife = life;
	this->color = glm::vec4(r, g, b, a);

    this->size = 4.0f; // 增大粒子大小，便于观察

    for(auto& h : hist) {
        h = position;
	}

}

Particle::Particle(glm::vec3 pos, glm::vec3 vel, float life, glm::vec4 color):position(pos),
velocity(vel),life(life),maxLife(life),color(color) {
    this->size = 4.0f; // 增大粒子大小，便于观察
    for (auto& h : hist) {
        h = position;
    }
}



void Particle::update(float deltaTime) {

    if (deltaTime >= histTimer) {
		hist[histHead] = position;
		histHead = (histHead + 1) % SNAP;
    }


    // 更新位置

	position += velocity * deltaTime;

    // 应用重力

    velocity -= Particle::GRAVITY * deltaTime;

    // 更新生命周期

    life -= deltaTime;

    // 更新透明度（随生命周期衰减）

    color.a = life / maxLife;
}

bool Particle::isAlive() const {

    return life > 0.0f;

}
