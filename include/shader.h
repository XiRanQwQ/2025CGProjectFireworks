#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:

    // 着色器程序ID

    unsigned int ID;

    // 构造函数

    Shader();
    Shader(const char* vertexPath, const char* fragmentPath);
    
    // 使用/激活着色器

    void use() const;
    
    // 设置uniform值的方法

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const glm::mat4& matrix) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;

private:

    // 检查着色器编译或链接错误

    void checkCompileErrors(unsigned int shader, std::string type);
};
