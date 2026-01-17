############################################################ 
# Computer Graphics Assignment Configuration 
# Tested on Windows 10 and Ubuntu 16.04.1 
# By Wencong Yang, yangwc3@mail2.sysu.edu.cn 
############################################################ 
cmake_minimum_required(VERSION 3.5) 

# You'd better build it in release mode for performance 
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE) 

project(CGFinalProject VERSION "1.0") 

# C++ 17 is required 
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 

# Include the project root directory 
include_directories(${PROJECT_SOURCE_DIR}/include) 
set(SOURCES
    src/main.cpp
    src/skybox.cpp
    src/camera.cpp
    src/particle.cpp
    src/particle_system.cpp
    src/firework.cpp
    src/shader.cpp
    src/audio.cpp
    src/ground.cpp
    src/tree.cpp
    src/utils.cpp
    src/post_processing.cpp
    src/fbo.cpp
    src/model.cpp
    src/mesh.cpp
)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)

# Find OpenAL
find_package(OpenAL REQUIRED)

if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL not found!")
endif()

if(NOT glfw3_FOUND)
    message(FATAL_ERROR "GLFW not found!")
endif()

if(NOT OPENAL_FOUND)
    message(FATAL_ERROR "OpenAL not found!")
endif()
# 添加可执行文件
add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
)
target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    OpenAL::OpenAL
    assimp::assimp
)
# 处理着色器文件
file(GLOB SHADER_SOURCES
    ${CMAKE_SOURCE_DIR}/shaders/*.vert
    ${CMAKE_SOURCE_DIR}/shaders/*.frag
)

# 将着色器文件复制到输出目录
foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(FILENAME ${SHADER} NAME)
    configure_file(${SHADER} ${CMAKE_BINARY_DIR}/shaders/${FILENAME} COPYONLY)
endforeach()

# 将音频文件复制到输出目录
set(AUDIO_FILES
    ${CMAKE_SOURCE_DIR}/launch.wav
    ${CMAKE_SOURCE_DIR}/explode.wav
)
foreach(AUDIO ${AUDIO_FILES})
    get_filename_component(FILENAME ${AUDIO} NAME)
    configure_file(${AUDIO} ${CMAKE_BINARY_DIR}/${FILENAME} COPYONLY)
endforeach()


file(GLOB_RECURSE MODEL_FILES
    ${CMAKE_SOURCE_DIR}/model/*
)
foreach(MODEL_FILE ${MODEL_FILES})
    file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR}/model ${MODEL_FILE})
    configure_file(${MODEL_FILE} ${CMAKE_BINARY_DIR}/model/${REL_PATH} COPYONLY)
endforeach()
