#include"glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>  
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "particle_system.h"
#include "camera.h"
#include "skybox.h"
#include "tree.h"
#include "ground.h"
#include "audio.h"
#include "post_processing.h"
#include "model.h"



// 窗口尺寸

#include <filesystem>



const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;


// 最大烟花数量（增加数量，便于测试）


const int MAX_FIREWORKS = 20;


// 全局变量


Camera* camera = nullptr;
Skybox* skybox = nullptr;
Ground* ground = nullptr;
Tree* tree = nullptr;
PostProcessing* postProcessing = nullptr;
Model* sceneModel = nullptr;
Shader* modelShader = nullptr;

// 鼠标状态


bool firstMouse = true;
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;


//键盘状态

static bool wasPressed_F = false;
static bool wasPressed_G = false;
static bool wasPressed_H = false;
static bool wasPressed_J = false;

// 相机移动速度


const float cameraSpeed = 50.0f;


// 鼠标灵敏度


const float mouseSensitivity = 0.001f;

// 森林构造参数

const int FOREST_SIZE_X = 20;     // X 方向多少棵
const int FOREST_SIZE_Z = 20;     // Z 方向多少棵
const float TREE_SPACING = 40.0f; // 基础间距
const float JITTER = 1.2f;       // 随机扰动范围
const float TREE_BASE_SCALE = 50.0f;  // 放大50 倍
constexpr float WORLD_GROUND_Y_OFFSET = -30.0f;

// 全局树木模型指针

struct TreeInstance {
    glm::vec3 position;
    float scale;
    float rotationY;
};

std::vector<TreeInstance> forest;

// 场景模型路径

constexpr bool LOAD_GLTF = false; 
constexpr const char* MODEL_PATH = LOAD_GLTF
    ? "model/building/scene.gltf"
    : "model/source/combined02.obj";


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

    //std::cout << "Current working directory: "
    //    << std::filesystem::current_path().string()
    //    << std::endl;

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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    // 启用混合

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // 初始化相机 - 调整位置以确保能看到烟花

    camera = new Camera(
        glm::vec3(-200.0f, 10.0f, 0.0f),
        glm::vec3(0.0f, 10.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        60.0f,
        (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT
    );


    // 初始化天空盒

    std::vector<std::string> skyboxTextures = {
        "resources/skybox/right.jpg",
        "resources/skybox/left.jpg",
        "resources/skybox/top.jpg",
        "resources/skybox/bottom.jpg",
        "resources/skybox/front.jpg",
        "resources/skybox/back.jpg"
    };

    skybox = new Skybox();
    if (!skybox->init(skyboxTextures)) {
        std::cerr << "Failed to initialize skybox" << std::endl;
        return -1;
    }


   

    // 初始化地面

    ground = new Ground(1000.0f);

    if (ground == nullptr) {
        std::cerr << "ground is nullptr!" << std::endl;
        return -1;
    }

    if (!ground->init()) {
        std::cerr << "Failed to initialize ground" << std::endl;
        return -1;
    }

    // 初始化树木

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    tree = new Tree();

    if (!tree->init()) {
        std::cerr << "Failed to initialize tree" << std::endl;
        return -1;
    }

    // 初始化通用模型（支持 glTF/OBJ），通过 LOAD_GLTF 切换
    
    try {
        modelShader = new Shader("shaders/model.vert", "shaders/model.frag");
        sceneModel = new Model(MODEL_PATH);
        std::cout << "[Model] loaded: " << MODEL_PATH << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to init model: " << e.what() << std::endl;
    }


    // 生成森林 

    for (int x = 0; x < FOREST_SIZE_X; ++x)
    {
        for (int z = 0; z < FOREST_SIZE_Z; ++z)
        {
            float jitterX = ((rand() % 100) / 100.0f - 0.5f) * JITTER;
            float jitterZ = ((rand() % 100) / 100.0f - 0.5f) * JITTER;

            TreeInstance t;
            t.position = glm::vec3(
                x * TREE_SPACING + jitterX,
                0.0f,
                z * TREE_SPACING + jitterZ
            );

            // 随机缩放
            t.scale = 0.8f + (rand() % 100) / 100.0f * 0.5f;

            // 随机 Y 轴旋转
            t.rotationY = (rand() % 360) * glm::radians(1.0f);

            forest.push_back(t);
        }
    }




    // 初始化音频系统
    if (!AudioManager::getInstance().init()) {
        std::cerr << "Failed to initialize audio system" << std::endl;
    }

    // 初始化粒子系统

    ParticleSystem particleSystem(MAX_FIREWORKS);
    particleSystem.init();


    // 初始化后处理效果
    postProcessing = new PostProcessing(WINDOW_WIDTH, WINDOW_HEIGHT);
    postProcessing->setBloomThreshold(0.7f);
    postProcessing->setBloomBlurSize(1.5f);
    postProcessing->setBloomIntensity(1.5f);
    postProcessing->setBloomPasses(3);

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
            camera->move(0.0f, cameraSpeed * deltaTime, 0.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera->move(0.0f, -cameraSpeed * deltaTime, 0.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera->move(0.0f, 0.0f, cameraSpeed * deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            camera->move(0.0f, 0.0f, -cameraSpeed * deltaTime);
        }
        // 单次触发，按一下 F 发一颗
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            if (!wasPressed_F) {
                particleSystem.addFirework();   // 立即调用你现成的接口
                wasPressed_F = true;
            }
        }
        else {

            wasPressed_F = false;
        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            if (!wasPressed_G) {
                particleSystem.addFirework(scatter);
                wasPressed_G = true;
            }
        }
        else {
            wasPressed_G = false;
        }
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
            if (!wasPressed_H) {
                particleSystem.addFirework(star);
                wasPressed_H = true;
            }
        }
        else {
            wasPressed_H = false;
        }
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
            if (!wasPressed_J) {
                particleSystem.addFirework(flower);
                wasPressed_J = true;
            }

        }
        else {
            wasPressed_J = false;
        }



        // 更新相机

        camera->update();

        // 开始后处理渲染（渲染到FBO）
        //postProcessing->beginRender();

        // 清空颜色和深度缓冲

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //test

        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = camera->getProjectionMatrix();
        /*std::cout << " (View Matrix) : " << view[3][3] << std::endl;
        std::cout << " (Projection Matrix) : " << projection[3][3] << std::endl;*/


        // 渲染地面

        ground->render(view, projection, WORLD_GROUND_Y_OFFSET);


        // 渲染天空盒



        skybox->render(view, projection);

        // 更新音频系统
        AudioManager::getInstance().update();

        // 更新粒子系统

        particleSystem.update(deltaTime);


        // 渲染通用模型
        
        if (sceneModel && modelShader)
        {
            modelShader->use();
            modelShader->setMat4("view", view);
            modelShader->setMat4("projection", projection);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(
                model,
                glm::vec3(0.0f, WORLD_GROUND_Y_OFFSET, 0.0f)
            );
            model = glm::scale(model, glm::vec3(0.1f)); // 如需缩放模型可调整此处

            modelShader->setMat4("model", model);
            sceneModel->Draw(*modelShader);
        }




        // 渲染树木

        for (const auto& t : forest)
        {
            glm::mat4 model = glm::mat4(1.0f);

            /*model = glm::translate(model, t.position);*/
            model = glm::translate(
                model,
                glm::vec3(
                    t.position.x,
                    t.position.y + WORLD_GROUND_Y_OFFSET,
                    t.position.z
                )
            );

            model = glm::rotate(model, t.rotationY, glm::vec3(0, 1, 0));
            model = glm::scale(model, glm::vec3(TREE_BASE_SCALE * t.scale));

            tree->draw(
                camera->getViewMatrix(),
                camera->getProjectionMatrix(),
                model
            );
        }


        glDepthMask(GL_FALSE);
        glEnable(GL_DEPTH_TEST);
        // 渲染粒子系统
        particleSystem.render(camera->getViewMatrix(), camera->getProjectionMatrix());

        glDepthMask(GL_TRUE);





        // 更新音频系统
        AudioManager::getInstance().update();
        // 交换缓冲

        glfwSwapBuffers(window);
    }


    // 清理

    delete camera;
    delete skybox;
    delete ground;
    delete tree;
    delete sceneModel;
    delete modelShader;


    // 关闭音频系统
    AudioManager::getInstance().shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
