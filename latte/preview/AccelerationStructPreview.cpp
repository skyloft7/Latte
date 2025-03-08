#include "AccelerationStructPreview.h"

#include <chrono>
#include <iostream>
#include <thread>
#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_glfw.h"
#include "../external/imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Core.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>


static int createShader(std::string name, std::string source, GLenum shaderType) {

    GLenum shader = glCreateShader(shaderType);

    const char* shaderSource = source.c_str();


    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    int shaderCompileSuccess;
    char shaderCompileInfoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompileSuccess);

    try {
        if(!shaderCompileSuccess) {
            glGetShaderInfoLog(shader, 512, NULL, shaderCompileInfoLog);
            throw std::runtime_error("Failed to compile shader " + name + ": \n" + shaderCompileInfoLog);
        }
    }
    catch (std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }


    return shader;
}
static void glfwErrorCallback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void AccelerationStructPreview::run(std::string title, int width, int height, std::shared_ptr<Mesh> mesh, std::shared_ptr<Visualizer> visualizer) {


    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) {
        return;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);




    GLFWwindow* window = glfwCreateWindow(1280, 720, "Latte Visualizer Tool", nullptr, nullptr);
    if (window == nullptr)
        return;


    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);



    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clearColor = ImVec4(0.4, 0.4, 0.4, 1.00f);




    glEnable(GL_DEPTH_TEST);




    uint32_t modelVao;
    uint32_t modelVbo;
    uint32_t modelIbo;
    uint32_t modelVertexShader;
    uint32_t modelFragmentShader;
    uint32_t modelShaderProgram;


    {

        glGenVertexArrays(1, &modelVao);
        glBindVertexArray(modelVao);

        glGenBuffers(1, &modelVbo);
        glBindBuffer(GL_ARRAY_BUFFER, modelVbo);
        glBufferData(GL_ARRAY_BUFFER, mesh->getVertices()->size() * sizeof(Vertex), &(mesh->getVertices()->at(0)), GL_STREAM_DRAW);


        glGenBuffers(1, &modelIbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->getIndices()->size() * sizeof(int), &(mesh->getIndices()->at(0)), GL_STREAM_DRAW);

        modelShaderProgram = glCreateProgram();

        modelVertexShader = createShader("ModelPreview_Vert", Core::readFile("shaders/preview/ModelPreview.vert"), GL_VERTEX_SHADER);
        modelFragmentShader = createShader("ModelPreview_Frag", Core::readFile("shaders/preview/ModelPreview.frag"), GL_FRAGMENT_SHADER);

        glAttachShader(modelShaderProgram, modelVertexShader);
        glAttachShader(modelShaderProgram, modelFragmentShader);
        glLinkProgram(modelShaderProgram);

        int strideBytes = 7 * sizeof(float);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, strideBytes, static_cast<void*> (nullptr));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideBytes, reinterpret_cast<void*>(4 * sizeof(float)));
        glEnableVertexAttribArray(1);

    }

    uint32_t visualizationVao;
    uint32_t visualizationVbo;
    uint32_t visualizationIbo;
    uint32_t visualizationVertexShader;
    uint32_t visualizationFragmentShader;
    uint32_t visualizationShaderProgram;
    std::shared_ptr<Mesh> visualizationMesh = visualizer->generate(*mesh);


    {


        glGenVertexArrays(1, &visualizationVao);
        glBindVertexArray(visualizationVao);

        glGenBuffers(1, &visualizationVbo);
        glBindBuffer(GL_ARRAY_BUFFER, visualizationVbo);
        glBufferData(GL_ARRAY_BUFFER, visualizationMesh->getVertices()->size() * sizeof(Vertex), &(visualizationMesh->getVertices()->at(0)), GL_STREAM_DRAW);

        glGenBuffers(1, &visualizationIbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, visualizationIbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, visualizationMesh->getIndices()->size() * sizeof(int), &(visualizationMesh->getIndices()->at(0)), GL_STREAM_DRAW);

        visualizationShaderProgram = glCreateProgram();

        visualizationVertexShader = createShader("Visualization_Vert", Core::readFile("shaders/preview/Visualization.vert"), GL_VERTEX_SHADER);
        visualizationFragmentShader = createShader("Visualization_Frag", Core::readFile("shaders/preview/Visualization.frag"), GL_FRAGMENT_SHADER);

        glAttachShader(visualizationShaderProgram, visualizationVertexShader);
        glAttachShader(visualizationShaderProgram, visualizationFragmentShader);
        glLinkProgram(visualizationShaderProgram);

        int strideBytes = 7 * sizeof(float);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, strideBytes, static_cast<void*> (nullptr));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideBytes, reinterpret_cast<void*>(4 * sizeof(float)));
        glEnableVertexAttribArray(1);


    }








    float scale = 0.01;
    auto transform = glm::mat4(1.0f) = glm::scale(glm::mat4(1.0), glm::vec3(scale));

    glm::vec3 whereToLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross((whereToLookAt - cameraPos), up));

    glm::mat4 view = glm::lookAt(cameraPos,
                 whereToLookAt,
                 up);

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);













    glm::vec2 mousePos = glm::vec2(0.0);
    glm::vec2 dragStartPos = glm::vec2(0.0);

    bool startedDrag = false;
    float currentRotX = 0, currentRotY = 0;
    float damping = 0.1;


    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::Begin(title.c_str());

        if (ImGui::SliderFloat("Scale", &scale, 0.0f, 1.0f)) {
            transform = glm::mat4(1.0f) = glm::scale(glm::mat4(1.0), glm::vec3(scale));
        }

        ImGui::ColorEdit3("Clear Color", reinterpret_cast<float*> (&clearColor));

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //Render the model
        {
            glBindVertexArray(modelVao);

            glUseProgram(modelShaderProgram);

            int transformLocation = glGetUniformLocation(modelShaderProgram, "transform");
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));

            int viewLocation = glGetUniformLocation(modelShaderProgram, "view");
            glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

            int projLocation = glGetUniformLocation(modelShaderProgram, "projection");
            glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));





            if (!io.WantCaptureMouse) {
                bool dragging = false;

                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
                    double mouseX = 0.0;
                    double mouseY = 0.0;
                    glfwGetCursorPos(window, &mouseX, &mouseY);

                    dragging = (mouseX != mousePos.x || mouseY != mousePos.y);
                    mousePos = glm::vec2((float) mouseX, (float) mouseY);

                    if (!startedDrag) {
                        startedDrag = true;
                        dragStartPos = mousePos;
                    }

                }
                else startedDrag = false;


                if (dragging) {

                    double newCursorX = 0.0;
                    double newCursorY = 0.0;
                    glfwGetCursorPos(window, &newCursorX, &newCursorY);
                    glm::vec2 newMousePos = glm::vec2(newCursorX, newCursorY);

                    if (newMousePos.x != dragStartPos.x && newMousePos.y != dragStartPos.y) {

                        std::this_thread::sleep_for(std::chrono::milliseconds(1));

                        double newestCursorX = 0.0;
                        double newestCursorY = 0.0;
                        glfwGetCursorPos(window, &newestCursorX, &newestCursorY);
                        glm::vec2 newestMousePos = glm::vec2(newestCursorX, newestCursorY);

                        float xsign = glm::sign(newestMousePos.x - newMousePos.x);
                        float ysign = glm::sign(newestMousePos.y - newMousePos.y);

                        float radX, radY;


                        float adjacent = glm::length(whereToLookAt.z - cameraPos.z);


                        {
                            float opposite = glm::length(newMousePos.x - dragStartPos.x);
                            radX = glm::atan(opposite / adjacent) * xsign;
                        }

                        {
                            float opposite = glm::length(newMousePos.y - dragStartPos.y);
                            radY = glm::atan(opposite / adjacent) * ysign;
                        }


                        transform = glm::rotate(transform, radX * damping, up);
                        transform = glm::rotate(transform, radY * damping, right);

                        currentRotX += radX * damping;
                        currentRotY += radY * damping;





                    }




                }
            }

            glBindBuffer(GL_ARRAY_BUFFER, modelVbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIbo);

            glDrawElements(GL_TRIANGLES, mesh->getIndices()->size(), GL_UNSIGNED_INT, nullptr);
        }

        //Render the visualization
        {

            glBindVertexArray(visualizationVao);

            glUseProgram(visualizationShaderProgram);

            int transformLocation = glGetUniformLocation(visualizationShaderProgram, "transform");
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));

            int viewLocation = glGetUniformLocation(visualizationShaderProgram, "view");
            glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

            int projLocation = glGetUniformLocation(visualizationShaderProgram, "projection");
            glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));

            glBindBuffer(GL_ARRAY_BUFFER, visualizationVbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, visualizationIbo);


            glDrawElements(GL_LINES, visualizationMesh->getIndices()->size(), GL_UNSIGNED_INT, nullptr);


        }


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();



















}