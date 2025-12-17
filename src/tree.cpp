#include "tree.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include <iostream>

// 顶点数据包含 position 与 texcoord

static float vertices[] = {

    // ===================== 树干：长方体（4个侧面） =====================

    // 前面 (+Z)

    -0.1f, 0.0f,  0.1f,   0.0f, 0.0f,
     0.1f, 0.0f,  0.1f,   1.0f, 0.0f,
     0.1f, 0.4f,  0.1f,   1.0f, 1.0f,

    -0.1f, 0.0f,  0.1f,   0.0f, 0.0f,
     0.1f, 0.4f,  0.1f,   1.0f, 1.0f,
    -0.1f, 0.4f,  0.1f,   0.0f, 1.0f,

    // 后面 (-Z)

    -0.1f, 0.0f, -0.1f,   0.0f, 0.0f,
    -0.1f, 0.4f, -0.1f,   0.0f, 1.0f,
     0.1f, 0.4f, -0.1f,   1.0f, 1.0f,

    -0.1f, 0.0f, -0.1f,   0.0f, 0.0f,
     0.1f, 0.4f, -0.1f,   1.0f, 1.0f,
     0.1f, 0.0f, -0.1f,   1.0f, 0.0f,

     // 左面 (-X)

     -0.1f, 0.0f, -0.1f,   0.0f, 0.0f,
     -0.1f, 0.0f,  0.1f,   1.0f, 0.0f,
     -0.1f, 0.4f,  0.1f,   1.0f, 1.0f,

     -0.1f, 0.0f, -0.1f,   0.0f, 0.0f,
     -0.1f, 0.4f,  0.1f,   1.0f, 1.0f,
     -0.1f, 0.4f, -0.1f,   0.0f, 1.0f,

     // 右面 (+X)

      0.1f, 0.0f, -0.1f,   0.0f, 0.0f,
      0.1f, 0.4f,  0.1f,   1.0f, 1.0f,
      0.1f, 0.0f,  0.1f,   1.0f, 0.0f,

      0.1f, 0.0f, -0.1f,   0.0f, 0.0f,
      0.1f, 0.4f, -0.1f,   0.0f, 1.0f,
      0.1f, 0.4f,  0.1f,   1.0f, 1.0f,

      // ===================== 树冠 =====================

      // 前面

      -0.3f, 0.35f, -0.3f,    0.0f, 0.0f,
       0.3f, 0.35f, -0.3f,   10.0f, 0.0f,
       0.0f, 1.0f,  0.0f,     5.0f, 10.0f,

       // 右面

        0.3f, 0.35f, -0.3f,    0.0f, 0.0f,
        0.3f, 0.35f,  0.3f,   10.0f, 0.0f,
        0.0f, 1.0f,  0.0f,     5.0f, 10.0f,

        // 后面

         0.3f, 0.35f,  0.3f,    0.0f, 0.0f,
        -0.3f, 0.35f,  0.3f,   10.0f, 0.0f,
         0.0f, 1.0f,  0.0f,     5.0f, 10.0f,

         // 左面

         -0.3f, 0.35f,  0.3f,    0.0f, 0.0f,
         -0.3f, 0.35f, -0.3f,   10.0f, 0.0f,
          0.0f, 1.0f,  0.0f,     5.0f, 10.0f,
};


Tree::Tree()
    : VAO(0), VBO(0), trunkTextureID(0), shader(nullptr)
{
}

Tree::~Tree()
{
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (trunkTextureID) glDeleteTextures(1, &trunkTextureID);
    if (shader) delete shader;
}

bool Tree::init()
{
    // 静态指定树干纹理路径

    const std::string trunkTexturePath = "assets/bark.jpg";

    std::cout << "Loading tree texture from: " << trunkTexturePath << std::endl;

    // 创建着色器

    try {
        shader = new Shader("shaders/tree.vert", "shaders/tree.frag");
        //if (!shader->isCompiled()) {
        //    std::cout << "Tree shader compile failed!" << std::endl;
        //    return false;
        //}

        std::cout << "Tree shader created successfully" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Tree shader creation failed: " << e.what() << std::endl;
        return false;
    }

    // 创建 VAO 和 VBO

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 顶点位置属性

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 纹理坐标属性

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // 加载树干纹理

    int width, height, channels;

	// 翻转图像y轴以匹配OpenGL纹理坐标系
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(trunkTexturePath.c_str(), &width, &height, &channels, 0);

    if (data)
    {

		// 设置纹理格式
        GLenum format = GL_RGB;
        if (channels == 1)
            format = GL_RED;
        else if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;

        std::cout << "Tree texture loaded: " << width << "x" << height
            << ", channels: " << channels << std::endl;


		// 创建 OpenGL 纹理对象
        glGenTextures(1, &trunkTextureID);
        glBindTexture(GL_TEXTURE_2D, trunkTextureID);

		// 设置纹理包裹方式和参数
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 上传纹理数据并生成 mipmaps
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 解绑
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        std::cout << "Cannot load tree texture: " << trunkTexturePath << std::endl;
        std::cout << "Error: " << stbi_failure_reason() << std::endl;
        return false;
    }

    stbi_image_free(data);
    return true;
}

void Tree::draw(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model)
{

    //std::cout << "=== tree draw test ===" << std::endl;

    if (!shader) {
        std::cerr << "Tree shader not initialized!" << std::endl;
        return;
    }

    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setMat4("model", model);

    glBindVertexArray(VAO);

    // 绘制树干（使用纹理）

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, trunkTextureID);
    shader->setInt("trunkTex", 0);
    shader->setBool("useTexture", true);  // 添加一个uniform控制

	glDrawArrays(GL_TRIANGLES, 0, 24); // 4个侧面，每面2个三角形，共24个顶点

    // 绘制树叶（纯色，不使用纹理）

    shader->setBool("useTexture", false);
    //shader->setVec3("leafColor", glm::vec3(0.2f, 0.8f, 0.2f));

    GLint locLeafColor = glGetUniformLocation(shader->ID, "leafColor");

    glm::vec3 leafColor(0.1f, 0.4f, 0.1f);  // 绿色
    glUniform3fv(locLeafColor, 1, glm::value_ptr(leafColor));

	// 绘制树冠部分（从第25个顶点开始，共12个顶点）

    glDrawArrays(GL_TRIANGLES, 24, 12);  

    glBindVertexArray(0);
}
