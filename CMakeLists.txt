cmake_minimum_required(VERSION 3.30)
project(Latte)

set(CMAKE_CXX_STANDARD 20)

option(ENABLE_PATHTRACER "Build the Latte PathTracer" ON)
option(ENABLE_VISUALIZER "Build the Latte Visualizer" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(PATHTRACER_SOURCES
        latte/core/accel/bvh/BVH.cpp
        latte/core/Main.cpp
        latte/core/Mesh.cpp
        latte/core/PixelBuffer.cpp
        latte/core/Raylet.cpp
        latte/core/RenderDevice.cpp
        latte/core/Renderer.cpp
        latte/Core.cpp
)

set(VISUALIZER_SOURCES
        latte/core/accel/bvh/BVH.cpp
        latte/preview/BVHVisualizer.cpp
        latte/core/Mesh.cpp
        latte/Core.cpp
        latte/external/glad/glad.c
        latte/external/imgui/imgui_tables.cpp
        latte/external/imgui/imgui_widgets.cpp
        latte/external/imgui/imgui_impl_opengl3.cpp
        latte/external/imgui/imgui_impl_glfw.cpp
        latte/external/imgui/imgui_draw.cpp
        latte/external/imgui/imgui_demo.cpp
        latte/external/imgui/imgui.cpp
        latte/preview/AccelerationStructPreview.cpp

        latte/Main.cpp
)


if (ENABLE_PATHTRACER)
    add_executable(LattePathTracer
            ${PATHTRACER_SOURCES}
    )
    include_directories(libs/glm-1.0.1-light/glm)
endif()

if (ENABLE_VISUALIZER)
    add_executable(LatteVisualizer
            ${VISUALIZER_SOURCES}
    )

    include_directories(libs/glm-1.0.1-light/glm libs/glfw-3.4.bin.WIN64/include/ libs/glad/include/)
    target_link_libraries(LatteVisualizer ${CMAKE_SOURCE_DIR}/libs/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.lib)
endif()
