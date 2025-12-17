#include "fbo.h"
#include <iostream>

FBO::FBO(int width, int height, bool useDepth)
    : width(width), height(height), useDepth(useDepth)
{
    createFBO();
}

FBO::~FBO()
{
    deleteFBO();
}

void FBO::createFBO()
{
    // 创建FBO
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // 创建颜色纹理
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    // 创建深度纹理（如果需要）
    if (useDepth) {
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    }

    // 检查FBO是否完整
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Failed to create FBO!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::deleteFBO()
{
    glDeleteTextures(1, &colorTexture);
    if (useDepth) {
        glDeleteTextures(1, &depthTexture);
    }
    glDeleteFramebuffers(1, &fbo);
}

void FBO::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
}

void FBO::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::resize(int newWidth, int newHeight)
{
    // 验证输入参数
    if (newWidth <= 0 || newHeight <= 0) {
        std::cerr << "FBO::resize: Invalid dimensions!" << std::endl;
        return;
    }

    if (newWidth == width && newHeight == height) {
        return;
    }

    width = newWidth;
    height = newHeight;

    deleteFBO();
    createFBO();
}