#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "firework.h"
#include "shader.h"
#include <vector>
#include <random>
#include <glad/glad.h>
#include <glm/glm.hpp>


// 粒子系统类

class ParticleSystem {

public:

    // 构造函数

    ParticleSystem(int maxFireworks);
    
    // 初始化

    void init();
    
    // 更新

    void update(float deltaTime);
    
    // 渲染

    void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    
    // 添加烟花

    void addFirework();
    
	// 添加粒子数据用于渲染

    void addParticleForRendering(std::vector<float>& particleData, const Particle& particle);

    void createGradientTexture();

    void generateAndBindTrailTexture(int size , float density);


private:

    // 烟花列表

    std::vector<Firework> fireworks;
    
    // 最大烟花数量

    int maxFireworks;
    
    // 着色器

    Shader* particleShader;
    
	Shader* rocketShader;
    // VAO和VBO

    unsigned int VAO, VBO;
    
    // 粒子数据

    std::vector<float> particleData;
	std::vector<float> rocketData;

	// 拖尾纹理ID

    GLuint texGradient = 0;
    GLuint trailTextureID;
    
    // 最大粒子数量

    const int MAX_PARTICLES = 200000;
};

#endif // PARTICLE_SYSTEM_H
