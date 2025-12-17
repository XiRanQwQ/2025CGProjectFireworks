#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ground.h"
#include <iostream>

//Ground* ground = nullptr;

Ground::Ground(float size)
{
    this->size = size;
    VAO = 0;
    VBO = 0;
    textureID = 0;
    shader = nullptr;
}

Ground::~Ground()
{
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (textureID) glDeleteTextures(1, &textureID);
    if (shader) delete shader;
}


// 初始化

bool Ground::init()
{
    //shader = new Shader(
    //    "D:/CGFinalProject/CGFP/shaders/ground.vert",
    //    "D:/CGFinalProject/CGFP/shaders/ground.frag"
    //);

    //if (!shader->isCompiled()) {
    //    std::cout << "Ground shader compile failed!" << std::endl;
    //    return false;
    //}

    // 地面半边长
    float s = size;

    // 顶点 (x,y,z , u,v)

    //float vertices[] = {
    //    -s, 0.0f, -s,    0.0f, 0.0f,
    //     s, 0.0f, -s,    1.0f, 0.0f,
    //     s, 0.0f,  s,    1.0f, 1.0f,

    //     s, 0.0f,  s,    1.0f, 1.0f,
    //    -s, 0.0f,  s,    0.0f, 1.0f,
    //    -s, 0.0f, -s,    0.0f, 0.0f
    //};
  
	// 为了让纹理重复铺设，调整uv坐标
    float scale = 10.0f;

    float vertices[] = {
        -s, 0.0f, -s,    0.0f,       0.0f,
         s, 0.0f, -s,    scale,      0.0f,
         s, 0.0f,  s,    scale,      scale,

         s, 0.0f,  s,    scale,      scale,
        -s, 0.0f,  s,    0.0f,       scale,
        -s, 0.0f, -s,    0.0f,       0.0f
    };

    
    // VAO VBO

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 位置属性

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV属性

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    // 纹理加载

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 包裹方式

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 过滤设置

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        GL_LINEAR);

    // 加载草地纹理

    int w, h, ch;
    unsigned char* data = stbi_load(
        "D:/CGFP ver1/assets/grass.jpg",
        &w, &h, &ch, 0);

    if (data)
    {
        // 上传到GPU

        GLenum format = GL_RGB;
        if (ch == 4) format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format,
            w, h, 0, format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Cannot load ground texture!" << std::endl;
        return false;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}



// 渲染

void Ground::render(const glm::mat4& view, const glm::mat4& projection, float yOffset)
{

	//std::cout << "=== ground render test ===" << std::endl;

    // 检查OpenGL错误

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error before rendering: " << error << std::endl;
    }

	// 使用着色器

	static bool shaderInitialized = false;
	//static Shader* shader = nullptr;

    // shader的延迟创建

	if (!shaderInitialized) {
		try {
			shader = new Shader(
				//"D:/CGFinalProject/CGFP/shaders/ground.vert",
				//"D:/CGFinalProject/CGFP/shaders/ground.frag"

                "D:/CGFP/shaders/ground.vert",
                "D:/CGFP/shaders/ground.frag"

			);

			shaderInitialized = true;
			std::cout << "Ground shader create success" << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << "Ground shader create fail: " << e.what() << std::endl;
			return;
		}
	}

    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

	// 模型矩阵进行地面高度偏移
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, yOffset, 0.0f));
    shader->setMat4("model", model);

	// 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader->setInt("groundTex", 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
