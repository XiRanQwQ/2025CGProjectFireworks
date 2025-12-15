#include "camera.h"
#include <cmath>

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fov, float aspectRatio) {
    this->position = position;
    this->target = target;
    this->up = up;
    this->fov = fov;
    this->aspectRatio = aspectRatio;


    // 计算初始欧拉角


    glm::vec3 direction = glm::normalize(target - position);
    this->yaw = atan2(direction.z, direction.x);
    this->pitch = asin(direction.y);


    // 更新方向向量

    updateVectors();
}

void Camera::update() {

    // 更新方向向量

    updateVectors();


    // 更新目标点

    target = position + front;
}

void Camera::setPosition(glm::vec3 position) {
    this->position = position;
}

void Camera::setTarget(glm::vec3 target) {
    this->target = target;


    // 重新计算欧拉角


    glm::vec3 direction = glm::normalize(target - position);
    this->yaw = atan2(direction.z, direction.x);
    this->pitch = asin(direction.y);


    // 更新方向向量

    updateVectors();
}

void Camera::rotate(float yawOffset, float pitchOffset) {

    // 更新欧拉角

    yaw += yawOffset;
    pitch += pitchOffset;


    // 限制俯仰角，避免翻转

    if (pitch > 1.5f) pitch = 1.5f;
    if (pitch < -1.5f) pitch = -1.5f;


    // 更新方向向量

    updateVectors();


    // 更新目标点

    target = position + front;
}

void Camera::move(float forward, float right, float up) {
    // 新增一行：水平化前向，抬头后仍往前走
    glm::vec3 flatFront = glm::normalize(glm::vec3(front.x, 0.f, front.z));

    position += flatFront * forward;          // 原来用 front，现在用 flatFront
    glm::vec3 worldRight = glm::normalize(glm::cross(glm::vec3(0, 1, 0), flatFront));
    position += worldRight * right;
    position += this->up * up;
    target = position + front;
}

void Camera::setFOV(float fov) {
    this->fov = fov;


    // 限制FOV范围

    if (fov < 1.0f) fov = 1.0f;
    if (fov > 120.0f) fov = 120.0f;
}

void Camera::updateVectors() {

    // 计算前向向量

    front.x = cos(yaw) * cos(pitch);
    front.y = sin(pitch);
    front.z = sin(yaw) * cos(pitch);
    front = glm::normalize(front);


    // 计算右向向量

    right = glm::normalize(glm::cross(front, up));


    // 更新上向量

    //this->up = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 1000.0f);
}

glm::vec3 Camera::getPosition() const {
    return position;
}

glm::vec3 Camera::getTarget() const {
    return target;
}
