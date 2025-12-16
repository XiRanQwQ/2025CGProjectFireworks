#include "particle_system.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <stb_image.h>
#include"utils.h"
#include "audio.h"

ParticleSystem::ParticleSystem(int maxFireworks)
    : maxFireworks(maxFireworks){}


void ParticleSystem::init()
{


    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);

    particleShader = new Shader("shaders/particle_trail.vert", "shaders/particle_trail.frag");
	rocketShader = new Shader("shaders/rocket.vert", "shaders/particle.frag");
    
    // Load firework audio files
    fireworkSoundBuffer = AudioManager::getInstance().loadWAV("resources/sound/launch.wav");
    if (fireworkSoundBuffer == 0) {
        std::cerr << "Failed to load launch.wav" << std::endl;
    }

    // 1. 新建 10×float 统一 VAO/VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 10 * sizeof(float), nullptr, GL_STREAM_DRAW);

    // 2. 5 分量映射（pos 3D + color 4D + life 1D + maxLife 1D + size 1D）
    constexpr int STRIDE = 10; 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, STRIDE * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, STRIDE * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, STRIDE * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, STRIDE * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glBindVertexArray(0);

    createGradientTexture();
	generateAndBindTrailTexture(64, 0.08f);
}

void ParticleSystem::createGradientTexture(){
    uint8_t px[64];
    for (int i = 0; i < 64; ++i)
        px[i] = 255 * exp(-(i / 63.0f) * (i / 63.0f) * 4.0f); // 中心白→边缘黑
    glGenTextures(1, &texGradient);
    glBindTexture(GL_TEXTURE_2D, texGradient);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 64, 1, 0, GL_RED, GL_UNSIGNED_BYTE, px);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void ParticleSystem::generateAndBindTrailTexture(int size = 64, float density = 0.08f) {
    std::vector<unsigned char> pixels(size * size * 4, 0); // RGBA

    for (int i = 0; i < size * size; ++i) {
        if (random(0.0f, 1.0f) < density) {
            float alpha = random(0.7f, 1.0f); // 随机亮度
            pixels[i * 4 + 0] = (unsigned char)(255 * alpha); // R
            pixels[i * 4 + 1] = (unsigned char)(255 * alpha); // G
            pixels[i * 4 + 2] = (unsigned char)(255 * alpha); // B
            pixels[i * 4 + 3] = (unsigned char)(255 * alpha); // A
        }
    }

    glGenTextures(1, &trailTextureID);
    glBindTexture(GL_TEXTURE_2D, trailTextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void ParticleSystem::update(float deltaTime) {



    // 更新所有烟花

    for (auto it = fireworks.begin(); it != fireworks.end();) {
        it->update(deltaTime);
        if (it->isFinished()) {
            it = fireworks.erase(it);
        }
        else {
            ++it;
        }
    }
}

void ParticleSystem::addParticleForRendering(std::vector<float>& particleData,const Particle& particle)
{
    // 5×float 结构：pos 3D + color 4D + life 1D + maxLife 1D + size 1D
    particleData.push_back(particle.position.x);
    particleData.push_back(particle.position.y);
    particleData.push_back(particle.position.z);

    particleData.push_back(particle.color.r);        // color.r
    particleData.push_back(particle.color.g);        // color.g
    particleData.push_back(particle.color.b);        // color.b
	particleData.push_back(particle.color.a);        // color.a

    particleData.push_back(particle.life);          //  life
    particleData.push_back(particle.maxLife);        // r 分量 = maxLife
    particleData.push_back(particle.size);           // g 分量 = size
}



void ParticleSystem::render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	rocketData.clear();
    particleData.clear();
    for (auto& fw : fireworks) {
        if (fw.getStatus() == LAUNCHING)
            addParticleForRendering(rocketData,fw.getRocket());
        for (const auto& p : fw.getParticles())
            addParticleForRendering(particleData,p);
    }

    // === 2. 渲染火箭（如果有）===
    if (!rocketData.empty()) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        rocketShader->use();
        rocketShader->setMat4("projection", projectionMatrix);
        rocketShader->setMat4("view", viewMatrix);
        rocketShader->setMat4("model", glm::mat4(1.0f));
        rocketShader->setBool("u_useTrail", true);
        //glActiveTexture(GL_TEXTURE0 + 1); // 使用合适的纹理单元
        //glBindTexture(GL_TEXTURE_2D, trailTextureID); // trailTextureID
        //rocketShader->setInt("u_trailMask", 1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
            rocketData.size() * sizeof(float),
            rocketData.data(),
            GL_DYNAMIC_DRAW); // 或 GL_STREAM_DRAW

        glBindVertexArray(VAO); // 确保 VAO layout 与 rocketShader 匹配！

        glDrawArrays(GL_POINTS, 0, rocketData.size() / 10); // 每粒子10个float

        glBindVertexArray(0);
    }

    // === 3. 渲染其他粒子）===
    if (!particleData.empty()) {
     
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
       
        particleShader->use();
        particleShader->setMat4("projection", projectionMatrix);
        particleShader->setMat4("view", viewMatrix);
        particleShader->setMat4("model", glm::mat4(1.0f));
        particleShader->setInt("u_gradient", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texGradient);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
            particleData.size() * sizeof(float),
            particleData.data(),
            GL_DYNAMIC_DRAW);

        glBindVertexArray(VAO); // 确保此 VAO 也兼容 particleShader 的 layout

        glDrawArrays(GL_POINTS, 0, particleData.size() / 10);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 恢复
        glBindVertexArray(0);
    }
}


void ParticleSystem::addFirework(FireworkType type) {

	std::cout << "Adding firework of type " << (type == common ? "common" : "scatter") << std::endl;

	if (fireworks.size() >= maxFireworks) return;
    // 1. 位置/速度/粒子数保持原样
    auto pos = glm::vec3(random(50.0f, 70.0f), 0.0f, random(-200.0f, 200.0f));
    auto vol = glm::vec3(0.0f, random(80.0f, 120.0f), 0.0f);
    int particleCount = random(150, 400);

    float baseHue = random(15.0f, 220.0f);   // 橙→黄
    float sat = random(0.7f, 0.95f);  // 不要太灰
    float val = random(0.85f, 1.0f);   // 亮度拉满
    glm::vec4 rocketColor = hsv2rgb(baseHue, sat, val);

    // 3. 再叠一层极小幅 RGB 抖动，让同批次也略有差别
    rocketColor.r = glm::clamp(rocketColor.r + random(-0.05f, 0.05f), 0.0f, 1.0f);
    rocketColor.g = glm::clamp(rocketColor.g + random(-0.05f, 0.05f), 0.0f, 1.0f);
    rocketColor.b = glm::clamp(rocketColor.b + random(-0.05f, 0.05f), 0.0f, 1.0f);

    // 3. 创建烟花
    fireworks.emplace_back(pos, vol, particleCount, rocketColor,type);

    // 4. 音频保持原样
    if (AudioManager::getInstance().getNumActiveSources() >= 16) return;
    if (fireworkSoundBuffer != 0)
        AudioManager::getInstance().playSound(fireworkSoundBuffer, 0.5f, 1.0f);
}
