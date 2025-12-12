#include"glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "particle_system.h"
#include "camera.h"
#include "skybox.h"



#include <filesystem> 



const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;


// 最大烟花数量（增加数量，便于测试）


const int MAX_FIREWORKS = 20;


// 全局变量


Camera* camera = nullptr;
Skybox* skybox = nullptr;


// 鼠标状态


bool firstMouse = true;
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;


// 相机移动速度


const float cameraSpeed = 50.0f;


// 鼠标灵敏度


const float mouseSensitivity = 0.001f;


// 键盘回调函数

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}


// 鼠标回调函数
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // 反向，因为y坐标从上到下递增

    lastX = xpos;
    lastY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    if (camera) {
        camera->rotate(xoffset, yoffset);
    }
}

int main() {

    std::cout << "Current working directory: "
        << std::filesystem::current_path().string()
        << std::endl;

    // 初始化GLFW


    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Firework Particle System", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }


    // 设置当前上下文

    glfwMakeContextCurrent(window);


    // 设置输入回调

    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);


    // 隐藏鼠标并捕获

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // 设置视口

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);


    // 启用深度测试

    glEnable(GL_DEPTH_TEST);


    // 设置清除颜色

    glClearColor(0.0f, 0.0f , 0.0f, 1.0f);
    glClear( GL_DEPTH_BUFFER_BIT);
    // 启用混合

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    // 初始化相机 - 调整位置以确保能看到烟花

    camera = new Camera(
        glm::vec3(50.0f, 5.0f, 0.0f), 
        glm::vec3(0.0f, 0.0f, 0.0f),     
        glm::vec3(0.0f, 1.0f, 0.0f),     
        60.0f,                           
        (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT  
    );


    // 初始化天空盒

    std::vector<std::string> skyboxTextures = {
        "resources/skybox/right.jpg",
        "resources/skybox/left.jpg",
        "resources/skybox/up.jpg",
        "resources/skybox/bottom.jpg",
        "resources/skybox/front.jpg",
        "resources/skybox/back.jpg"
    };

    skybox = new Skybox();
    if (!skybox->init(skyboxTextures)) {
        std::cerr << "Failed to initialize skybox" << std::endl;
        return -1;
    }


    // 初始化粒子系统

    ParticleSystem particleSystem(MAX_FIREWORKS);
    particleSystem.init();


    // 时间管理

    double lastTime = glfwGetTime();

    // 渲染循环



    while (!glfwWindowShouldClose(window)) {


        // 计算时间差

        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;



        // 处理输入

        glfwPollEvents();


        // 处理键盘输入

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera->move(cameraSpeed * deltaTime, 0.0f, 0.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera->move(-cameraSpeed * deltaTime, 0.0f, 0.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera->move(0.0f, -cameraSpeed * deltaTime, 0.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera->move(0.0f, cameraSpeed * deltaTime, 0.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera->move(0.0f, 0.0f, cameraSpeed * deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            camera->move(0.0f, 0.0f, -cameraSpeed * deltaTime);
        }


        // 更新相机

        camera->update();


        // 清空颜色和深度缓冲

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //test

        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = camera->getProjectionMatrix();
        /*std::cout << " (View Matrix) : " << view[3][3] << std::endl;
        std::cout << " (Projection Matrix) : " << projection[3][3] << std::endl;*/

        // 渲染天空盒

        //skybox->render(camera->getViewMatrix(), camera->getProjectionMatrix());

        skybox->render(view, projection);

        // 更新粒子系统

        particleSystem.update(deltaTime);



        // 渲染粒子系统

        glDepthMask(GL_FALSE);
        glEnable(GL_DEPTH_TEST);   


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



        particleSystem.render(camera->getViewMatrix(), camera->getProjectionMatrix());

        // 4. 恢复常规状态，再画场景物体
        glDepthMask(GL_TRUE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 交换缓冲

        glfwSwapBuffers(window);
    }


    // 清理

    delete camera;
    delete skybox;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
