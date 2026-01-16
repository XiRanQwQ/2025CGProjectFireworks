#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "shader.h"

// 单个顶点结构体

struct Vertex 
{
    glm::vec3 Position; // 定点坐标
	glm::vec3 Normal;   // 法线
	glm::vec2 TexCoords;    // 纹理坐标
};

// 纹理结构体

struct Texture 
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh 
{
public:
	std::vector<Vertex>       vertices; // 顶点数组
	std::vector<unsigned int> indices;  // 索引数组
	std::vector<Texture>      textures; // 纹理数组

	unsigned int VAO;   //绘制入口

    Mesh(std::vector<Vertex> vertices,
         std::vector<unsigned int> indices,
         std::vector<Texture> textures);

    void Draw(Shader& shader);

private:
    unsigned int VBO, EBO;
    void setupMesh();
};
