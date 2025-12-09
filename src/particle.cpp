#include "particle.h"

Particle::Particle(float x, float y, float z, float vx, float vy, float vz, float life, float r, float g, float b, float a) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->vx = vx;
    this->vy = vy;
    this->vz = vz;
    this->life = life;
    this->maxLife = life;
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;

    this->size = 4.0f; // 增大粒子大小，便于观察

}

void Particle::update(float deltaTime) {

    // 更新位置

    x += vx * deltaTime;
    y += vy * deltaTime;
    z += vz * deltaTime;

    // 应用重力

    vy -= gravity * deltaTime;

    // 更新生命周期

    life -= deltaTime;

    // 更新透明度（随生命周期衰减）

    a = life / maxLife;
}

bool Particle::isAlive() const {

    return life > 0.0f;

}
