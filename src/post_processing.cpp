#include "post_processing.h"
#include <iostream>

PostProcessing::PostProcessing(int width, int height)
    : width(width), height(height),
      bloomThreshold(0.5f), bloomBlurSize(1.0f), bloomIntensity(1.5f), bloomPasses(2)
{
    // 创建FBO
    sceneFBO = new FBO(width, height, true);
    pingPongFBO[0] = new FBO(width / 2, height / 2, false);
    pingPongFBO[1] = new FBO(width / 2, height / 2, false);

    // 加载着色器
    brightnessShader = new Shader("shaders/postprocess.vert", "shaders/brightness.frag");
    blurShader = new Shader("shaders/postprocess.vert", "shaders/blur.frag");
    bloomShader = new Shader("shaders/postprocess.vert", "shaders/bloom.frag");

    // 创建全屏四边形
    createFullscreenQuad();
}

PostProcessing::~PostProcessing()
{
    delete sceneFBO;
    delete pingPongFBO[0];
    delete pingPongFBO[1];
    delete brightnessShader;
    delete blurShader;
    delete bloomShader;

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}

void PostProcessing::createFullscreenQuad()
{
    // 全屏四边形的顶点数据
    float quadVertices[] = {
        // 位置          // 纹理坐标
        -1.0f,  1.0f,    0.0f, 1.0f,
        -1.0f, -1.0f,    0.0f, 0.0f,
         1.0f, -1.0f,    1.0f, 0.0f,

        -1.0f,  1.0f,    0.0f, 1.0f,
         1.0f, -1.0f,    1.0f, 0.0f,
         1.0f,  1.0f,    1.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    
    // 位置属性
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // 纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void PostProcessing::renderQuad() const
{
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcessing::beginRender()
{
    sceneFBO->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessing::endRender()
{
    sceneFBO->unbind();
}

void PostProcessing::applyBloom(float threshold, float blurSize, float intensity, int blurPasses)
{
    // 验证输入参数
    if (blurPasses < 0) {
        blurPasses = 0;
        std::cerr << "PostProcessing::applyBloom: Invalid blurPasses, setting to 0" << std::endl;
    }
    if (threshold < 0.0f) {
        threshold = 0.0f;
        std::cerr << "PostProcessing::applyBloom: Invalid threshold, setting to 0.0" << std::endl;
    }
    if (blurSize < 0.0f) {
        blurSize = 0.0f;
        std::cerr << "PostProcessing::applyBloom: Invalid blurSize, setting to 0.0" << std::endl;
    }
    if (intensity < 0.0f) {
        intensity = 0.0f;
        std::cerr << "PostProcessing::applyBloom: Invalid intensity, setting to 0.0" << std::endl;
    }
    
    // 更新参数
    bloomThreshold = threshold;
    bloomBlurSize = blurSize;
    bloomIntensity = intensity;
    bloomPasses = blurPasses;

    // 1. 提取亮部
    brightnessShader->use();
    brightnessShader->setFloat("threshold", bloomThreshold);
    brightnessShader->setInt("screenTexture", 0);
    
    pingPongFBO[0]->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneFBO->getColorTexture());
    renderQuad();
    pingPongFBO[0]->unbind();

    // 2. 应用高斯模糊
    blurShader->use();
    blurShader->setInt("screenTexture", 0);
    blurShader->setFloat("blurSize", bloomBlurSize);
    
    bool horizontal = true;
    for (int i = 0; i < bloomPasses; i++) {
        pingPongFBO[horizontal]->bind();
        blurShader->setVec2("direction", horizontal ? glm::vec2(1.0f, 0.0f) : glm::vec2(0.0f, 1.0f));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pingPongFBO[!horizontal]->getColorTexture());
        renderQuad();
        
        pingPongFBO[horizontal]->unbind();
        horizontal = !horizontal;
    }
}

void PostProcessing::renderFinal() const
{
    // 渲染最终场景与辉光效果
    bloomShader->use();
    bloomShader->setInt("screenTexture", 0);
    bloomShader->setInt("bloomTexture", 1);
    bloomShader->setFloat("bloomIntensity", bloomIntensity);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneFBO->getColorTexture());
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingPongFBO[0]->getColorTexture());
    
    renderQuad();
}

void PostProcessing::resize(int newWidth, int newHeight)
{
    // 验证输入参数
    if (newWidth <= 0 || newHeight <= 0) {
        std::cerr << "PostProcessing::resize: Invalid dimensions!" << std::endl;
        return;
    }

    if (newWidth == width && newHeight == height) {
        return;
    }

    width = newWidth;
    height = newHeight;

    // 调整所有FBO的大小
    sceneFBO->resize(width, height);
    pingPongFBO[0]->resize(width / 2, height / 2);
    pingPongFBO[1]->resize(width / 2, height / 2);
}