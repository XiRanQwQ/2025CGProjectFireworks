############################################################ 
# 烟花项目最终 CMake 配置（全依赖自包含，零系统依赖）
# 适配：无音频文件、glfw3.dll在system32中无需复制
############################################################ 
cmake_minimum_required(VERSION 3.5) 

# 强制Release模式编译（性能最优）
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE) 

# 项目名称&版本
project(CGFinalProject VERSION "1.0") 

# 强制开启C++17（项目要求）
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 

# ========== 1. 头文件目录（项目自身 + 所有依赖） ==========
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/vendor/include
)

# ========== 2. 源码列表（包含GLAD源码，必须加！） ==========
set(SOURCES
    # 项目核心源码
    src/main.cpp
    src/skybox.cpp
    src/camera.cpp
    src/particle.cpp
    src/particle_system.cpp
    src/firework.cpp
    src/shader.cpp
    src/audio.cpp  # 保留audio.cpp（若不需要可删除）
    src/ground.cpp
    src/tree.cpp
    src/utils.cpp
    src/post_processing.cpp
    src/fbo.cpp
    src/model.cpp
    src/mesh.cpp
    # GLAD源码（纯头文件库的实现文件，必须添加）
    src/glad.c
    src/stb_vorbis.c
)

# ========== 3. 链接库目录（仅项目内vendor/lib） ==========
link_directories(${PROJECT_SOURCE_DIR}/vendor/lib)

# ========== 4. 创建可执行文件 ==========
add_executable(
    ${PROJECT_NAME}  # 可执行文件名 = 项目名
    ${SOURCES}       # 所有源码文件
)

# ========== 5. 链接所有库文件 ==========
target_link_libraries(${PROJECT_NAME}
    opengl32.lib          # Windows系统自带OpenGL库
    glfw3.lib          # vendor/lib/glfw3.lib
    OpenAL32.lib          # vendor/lib/OpenAL32.lib（OpenAL库）
    assimp-vc143-mt.lib    # vendor/lib/assimp-vc143-mt.lib（assimp库）
)

# ========== 6. 自动复制运行时DLL到输出目录 ==========
# 【修改1：删除glfw3.dll复制（已在system32中）】
# configure_file(
#     ${PROJECT_SOURCE_DIR}/vendor/bin/glfw3.dll
#     ${CMAKE_BINARY_DIR}/glfw3.dll
#     COPYONLY
# )

configure_file(
    ${PROJECT_SOURCE_DIR}/vendor/bin/OpenAL32.dll
    ${CMAKE_BINARY_DIR}/Release/OpenAL32.dll
    COPYONLY
)
configure_file(
    ${PROJECT_SOURCE_DIR}/vendor/bin/assimp-vc143-mt.dll
    ${CMAKE_BINARY_DIR}/Release/assimp-vc143-mt.dll
    COPYONLY
)

# ========== 7. 复制资源到Release目录 ==========
# 着色器
file(GLOB SHADER_SOURCES ${CMAKE_SOURCE_DIR}/shaders/*.vert ${CMAKE_SOURCE_DIR}/shaders/*.frag)
foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(FILENAME ${SHADER} NAME)
    configure_file(${SHADER} ${CMAKE_BINARY_DIR}/shaders/${FILENAME} COPYONLY)
endforeach()

# 模型
file(GLOB_RECURSE MODEL_FILES ${CMAKE_SOURCE_DIR}/model/*)
foreach(MODEL_FILE ${MODEL_FILES})
    file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR}/model ${MODEL_FILE})
    configure_file(${MODEL_FILE} ${CMAKE_BINARY_DIR}/model/${REL_PATH} COPYONLY)
endforeach()

# assets
file(GLOB_RECURSE RESOURCE_FILES ${CMAKE_SOURCE_DIR}/assets/*)
foreach(RES_FILE ${RESOURCE_FILES})
    file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR}/assets ${RES_FILE})
    configure_file(${RES_FILE} ${CMAKE_BINARY_DIR}/assets/${REL_PATH} COPYONLY)
endforeach()

file(GLOB_RECURSE RESOURCE_FILES ${CMAKE_SOURCE_DIR}/resources/*)
foreach(RES_FILE ${RESOURCE_FILES})
    file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR}/resources ${RES_FILE})
    configure_file(${RES_FILE} ${CMAKE_BINARY_DIR}/resources/${REL_PATH} COPYONLY)
endforeach()