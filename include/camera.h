#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 相机类

class Camera {
    public:

    // 构造函数

    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fov, float aspectRatio);
    
    // 更新相机

    void update();
    
    // 设置位置

    void setPosition(glm::vec3 position);
    
    // 设置目标点

    void setTarget(glm::vec3 target);
    
    // 旋转相机（根据鼠标移动）

    void rotate(float yaw, float pitch);
    
    // 移动相机（根据键盘输入）

    void move(float forward, float right, float up);
    
    // 设置视角（FOV）

    void setFOV(float fov);
    
    // 获取视图矩阵

    glm::mat4 getViewMatrix() const;
    
    // 获取投影矩阵

    glm::mat4 getProjectionMatrix() const;
    
    // 获取相机位置

    glm::vec3 getPosition() const;
    
    // 获取相机目标

    glm::vec3 getTarget() const;
    
    private:

    // 相机参数

    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float fov;
    float aspectRatio;
    
    // 欧拉角

    float yaw;
    float pitch;
    
    // 相机方向

    glm::vec3 front;
    glm::vec3 right;
    
    // 更新相机方向

    void updateVectors();
};

#endif // CAMERA_H
