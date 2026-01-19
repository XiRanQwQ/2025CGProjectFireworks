#ifndef SKYBOX_H
#define SKYBOX_H

#include"glad/glad.h"
#include <GLFW/glfw3.h>
#include<glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "shader.h"


// 天空盒类

class Skybox {
public:

    
    GLuint getCloudTexture() const { return cloudTexture; }
    // 构造函数

    Skybox();
    
    // 析构函数

    ~Skybox();
    
    // 初始化天空盒

    bool init(const std::vector<std::string>& textureFaces);
    
    // 渲染天空盒

    void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    void Skybox::loadCloudTexture(const char* path);
    // 设置天空盒大小

    void setSize(float size);
    
    // 获取天空盒大小

    float getSize() const;
    
private:

    // 天空盒参数

    float size;
    GLuint VAO, VBO;

    GLuint cubeMapID;

    
    //立方体贴图ID
    
    // 天空盒顶点数据

    float vertices[108]; 
    
    // 立方体的6个面，每个面2个三角形，每个三角形3个顶点，每个顶点3个坐标

    GLuint loadCubeMap(const std::vector<std::string>& faces);
    //加载立方体贴图
    GLuint cloudTexture;
    // 初始化天空盒几何体

    void initGeometry();
};

#endif // SKYBOX_H
