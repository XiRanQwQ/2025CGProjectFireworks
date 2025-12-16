#ifndef FBO_H
#define FBO_H

#include <glad/glad.h>
#include <glm/glm.hpp>

/**
 * @brief Frame Buffer Object (FBO) 类，用于离屏渲染
 */
class FBO {
public:
    /**
     * @brief 构造函数
     * @param width FBO宽度
     * @param height FBO高度
     * @param useDepth 是否使用深度纹理
     */
    FBO(int width, int height, bool useDepth = true);
    ~FBO();

    /**
     * @brief 绑定FBO，开始渲染到该FBO
     */
    void bind() const;

    /**
     * @brief 解绑FBO，恢复渲染到默认帧缓冲
     */
    void unbind() const;

    /**
     * @brief 调整FBO大小
     * @param width 新宽度
     * @param height 新高度
     */
    void resize(int width, int height);

    /**
     * @brief 获取颜色纹理ID
     * @return 颜色纹理ID
     */
    GLuint getColorTexture() const { return colorTexture; }

    /**
     * @brief 获取深度纹理ID
     * @return 深度纹理ID
     */
    GLuint getDepthTexture() const { return depthTexture; }

private:
    /**
     * @brief 创建FBO及其附件
     */
    void createFBO();

    /**
     * @brief 删除FBO及其附件
     */
    void deleteFBO();

    GLuint fbo;          // FBO ID
    GLuint colorTexture; // 颜色纹理ID
    GLuint depthTexture; // 深度纹理ID
    int width;           // FBO宽度
    int height;          // FBO高度
    bool useDepth;       // 是否使用深度纹理
};

#endif // FBO_H