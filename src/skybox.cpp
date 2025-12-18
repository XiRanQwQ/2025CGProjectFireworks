#include "skybox.h"
#include <iostream>
#include<filesystem>
#include<fstream>

#include "stb_image.h"

//图片加载库

Skybox::Skybox() {

    // 初始化天空盒大小

    size = 500.0f;


    // 初始化顶点数组

    initGeometry();


    // 初始化OpenGL对象

    VAO = 0;
    VBO = 0;
    cubeMapID = 0;
}

Skybox::~Skybox() {

    // 释放OpenGL资源

    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
    }
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
    }
    if (cubeMapID != 0) {
        glDeleteTextures(1, &cubeMapID);
    }
}

GLuint Skybox::loadCubeMap(const std::vector<std::string>& faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    
    stbi_set_flip_vertically_on_load(false);
    
    // 不要翻转图像，因为立方体贴图的方向与普通2D纹理不同

    for (GLuint i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {

            GLenum internalFormat = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            GLenum dataFormat = (nrChannels == 4) ? GL_RGBA : GL_RGB;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    // 设置纹理参数

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // 解绑纹理

    return textureID;
}

bool Skybox::init(const std::vector<std::string>& faces) {

    // 初始化几何体

    initGeometry();

    // 创建VAO和VBO

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);


    // 检查VAO和VBO创建是否成功

    if (VAO == 0 || VBO == 0) {
        std::cerr << "Failed to create VAO or VBO" << std::endl;
        return false;
    }


    // 绑定VAO

    glBindVertexArray(VAO);


    // 绑定VBO并填充数据

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    // 设置顶点属性

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // 解绑VAO

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    cubeMapID = loadCubeMap(faces);
    if(cubeMapID == 0){
        std::cerr << "Failed to load cubemap texture" << std::endl;
        return false;
    }


    return true;
}

void Skybox::render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    // 保存当前深度测试状态

    GLint depthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);

    // 设置天空盒专用的深度测试

    glDepthFunc(GL_LEQUAL);  // 关键：改为小于或等于[3,6](@ref)
    glDepthMask(GL_FALSE);   // 禁用深度写入

    //std::cout << "=== skybox render test ===" << std::endl;

    // 检查OpenGL错误
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error before rendering: " << error << std::endl;
    }

    // 创建并使用天空盒着色器

    static bool shaderInitialized = false;
    static Shader skyboxShader;

    if (!shaderInitialized) {
        try {

            skyboxShader = Shader("shaders/skybox.vert",
                "shaders/skybox.frag");

            shaderInitialized = true;
            std::cout << "create success" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "create fail: " << e.what() << std::endl;
            // 恢复深度测试状态
            glDepthMask(GL_TRUE);
            glDepthFunc(depthFunc);
            return;
        }
    }

    skyboxShader.use();

    // 设置视图矩阵（去除平移部分，只保留旋转）[3](@ref)

    glm::mat4 skyboxView = glm::mat4(glm::mat3(viewMatrix));

    // 设置uniforms

    skyboxShader.setMat4("projection", projectionMatrix);
    skyboxShader.setMat4("view", skyboxView);

    // 绑定立方体贴图
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);
    skyboxShader.setInt("skybox", 0);

    // 绑定VAO并绘制

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // 恢复深度测试状态[3](@ref)

    glDepthMask(GL_TRUE);
    glDepthFunc(depthFunc);

    // 检查OpenGL错误

    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error after rendering: " << error << std::endl;
    }
}

void Skybox::setSize(float size) {
    this->size = size;
    initGeometry(); // 重新初始化几何体
}

float Skybox::getSize() const {
    return size;
}


void Skybox::initGeometry() {

    // 天空盒的顶点数据（立方体的6个面）

    float halfSize = size / 2.0f;


    // 前面

    vertices[0] = -halfSize; vertices[1] = -halfSize; vertices[2] = halfSize;
    vertices[3] = halfSize; vertices[4] = -halfSize; vertices[5] = halfSize;
    vertices[6] = halfSize; vertices[7] = halfSize; vertices[8] = halfSize;
    vertices[9] = halfSize; vertices[10] = halfSize; vertices[11] = halfSize;
    vertices[12] = -halfSize; vertices[13] = halfSize; vertices[14] = halfSize;
    vertices[15] = -halfSize; vertices[16] = -halfSize; vertices[17] = halfSize;


    // 后面

    vertices[18] = -halfSize; vertices[19] = -halfSize; vertices[20] = -halfSize;
    vertices[21] = halfSize; vertices[22] = -halfSize; vertices[23] = -halfSize;
    vertices[24] = halfSize; vertices[25] = halfSize; vertices[26] = -halfSize;
    vertices[27] = halfSize; vertices[28] = halfSize; vertices[29] = -halfSize;
    vertices[30] = -halfSize; vertices[31] = halfSize; vertices[32] = -halfSize;
    vertices[33] = -halfSize; vertices[34] = -halfSize; vertices[35] = -halfSize;


    // 左面

    vertices[36] = -halfSize; vertices[37] = halfSize; vertices[38] = -halfSize;
    vertices[39] = -halfSize; vertices[40] = halfSize; vertices[41] = halfSize;
    vertices[42] = -halfSize; vertices[43] = -halfSize; vertices[44] = halfSize;
    vertices[45] = -halfSize; vertices[46] = -halfSize; vertices[47] = halfSize;
    vertices[48] = -halfSize; vertices[49] = -halfSize; vertices[50] = -halfSize;
    vertices[51] = -halfSize; vertices[52] = halfSize; vertices[53] = -halfSize;


    // 右面

    vertices[54] = halfSize; vertices[55] = halfSize; vertices[56] = -halfSize;
    vertices[57] = halfSize; vertices[58] = halfSize; vertices[59] = halfSize;
    vertices[60] = halfSize; vertices[61] = -halfSize; vertices[62] = halfSize;
    vertices[63] = halfSize; vertices[64] = -halfSize; vertices[65] = halfSize;
    vertices[66] = halfSize; vertices[67] = -halfSize; vertices[68] = -halfSize;
    vertices[69] = halfSize; vertices[70] = halfSize; vertices[71] = -halfSize;


    // 下面

    vertices[72] = -halfSize; vertices[73] = -halfSize; vertices[74] = -halfSize;
    vertices[75] = halfSize; vertices[76] = -halfSize; vertices[77] = -halfSize;
    vertices[78] = halfSize; vertices[79] = -halfSize; vertices[80] = halfSize;
    vertices[81] = halfSize; vertices[82] = -halfSize; vertices[83] = halfSize;
    vertices[84] = -halfSize; vertices[85] = -halfSize; vertices[86] = halfSize;
    vertices[87] = -halfSize; vertices[88] = -halfSize; vertices[89] = -halfSize;


    // 上面

    vertices[90] = -halfSize; vertices[91] = halfSize; vertices[92] = -halfSize;
    vertices[93] = halfSize; vertices[94] = halfSize; vertices[95] = -halfSize;
    vertices[96] = halfSize; vertices[97] = halfSize; vertices[98] = halfSize;
    vertices[99] = halfSize; vertices[100] = halfSize; vertices[101] = halfSize;
    vertices[102] = -halfSize; vertices[103] = halfSize; vertices[104] = halfSize;
    vertices[105] = -halfSize; vertices[106] = halfSize; vertices[107] = -halfSize;

   /* std::cout << "skybox vert test:" << std::endl;
    std::cout << "point num: " << (sizeof(vertices) / sizeof(vertices[0])) / 3 << std::endl;
    for (int i = 0; i < 6; ++i) {
        int idx = i * 3;
        std::cout << "point " << i << ": (" << vertices[idx] << ", "
            << vertices[idx + 1] << ", " << vertices[idx + 2] << ")" << std::endl;
    }*/
}