#ifndef POST_PROCESSING_H
#define POST_PROCESSING_H

#include "fbo.h"
#include "shader.h"
#include <vector>

/**
 * @brief 后处理类，用于实现辉光效果
 */
class PostProcessing {
public:
    /**
     * @brief 构造函数
     * @param width 后处理宽度
     * @param height 后处理高度
     */
    PostProcessing(int width, int height);
    ~PostProcessing();

    /**
     * @brief 开始渲染到FBO
     */
    void beginRender();

    /**
     * @brief 结束渲染到FBO
     */
    void endRender();

    /**
     * @brief 应用辉光效果
     * @param threshold 亮度阈值
     * @param blurSize 模糊大小
     * @param intensity 辉光强度
     * @param blurPasses 模糊次数
     */
    void applyBloom(float threshold, float blurSize, float intensity, int blurPasses = 2);

    /**
     * @brief 渲染后处理效果到屏幕
     */
    void renderFinal() const;

    /**
     * @brief 调整后处理大小
     * @param width 新宽度
     * @param height 新高度
     */
    void resize(int width, int height);

    // 辉光参数控制
    void setBloomThreshold(float threshold) { bloomThreshold = threshold; }
    void setBloomBlurSize(float size) { bloomBlurSize = size; }
    void setBloomIntensity(float intensity) { bloomIntensity = intensity; }
    void setBloomPasses(int passes) { bloomPasses = passes; }

private:
    /**
     * @brief 创建全屏四边形用于渲染
     */
    void createFullscreenQuad();

    /**
     * @brief 渲染全屏四边形
     */
    void renderQuad() const;

    FBO* sceneFBO;           // 场景渲染FBO
    FBO* pingPongFBO[2];     // 用于高斯模糊的FBO
    
    Shader* brightnessShader; // 亮度提取着色器
    Shader* blurShader;       // 模糊着色器
    Shader* bloomShader;      // 辉光混合着色器
    
    GLuint quadVAO, quadVBO;  // 全屏四边形的VAO和VBO
    
    int width;                // 后处理宽度
    int height;               // 后处理高度
    
    // 辉光参数
    float bloomThreshold;     // 亮度阈值
    float bloomBlurSize;      // 模糊大小
    float bloomIntensity;     // 辉光强度
    int bloomPasses;          // 模糊次数
};

#endif // POST_PROCESSING_H