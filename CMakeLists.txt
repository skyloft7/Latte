cmake_minimum_required(VERSION 3.30)
project(Latte)

set(CMAKE_CXX_STANDARD 20)

add_executable(Latte latte/main.cpp
        latte/core/Mesh.cpp
        latte/preview/AccelerationStructPreview.cpp
        latte/preview/AccelerationStructPreview.h
        latte/external/glad.c
        latte/Core.h
        latte/Core.cpp
        latte/core/Vertex.h
        latte/preview/Visualizer.h
        latte/preview/BVHVisualizer.cpp
        latte/preview/BVHVisualizer.h
)

include_directories(libs/glm-1.0.1-light/glm libs/glfw-3.4.bin.WIN64/include/ libs/glad/include/)

target_link_libraries(Latte ${CMAKE_SOURCE_DIR}/libs/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.lib)
