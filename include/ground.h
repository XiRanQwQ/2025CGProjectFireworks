#pragma once
#include"glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"



class Ground
{
public:
    Ground(float size = 1000.0f);
    ~Ground();

    bool init();
    void render(const glm::mat4& view, const glm::mat4& projection, float yOffset);
    // void setSize(float s);

private:
    void initGeometry();
    //void initTexture();


    GLuint VAO, VBO;
    GLuint textureID;

    float size;
    //float vertices[30];   // 6 点，每点5个float (x,y,z,u,v)

    Shader* shader;
    //bool shaderInitialized;
};

//extern Ground* ground;
