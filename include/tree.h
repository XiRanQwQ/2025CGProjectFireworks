#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"

class Tree {
public:
    Tree();
    ~Tree();
    bool init();
    void draw(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model);

private:
    unsigned int VAO, VBO;

    void initGeometry();
    //void initShader();

    // Ê÷¸ÉÎÆÀí

    unsigned int trunkTextureID;

    Shader* shader;

    // Ê÷Ò¶
    // unsigned int leafTexture;
};
