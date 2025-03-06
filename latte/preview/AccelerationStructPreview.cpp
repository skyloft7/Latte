#include "AccelerationStructPreview.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <Windows.h>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Core.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>
#include <numbers>

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            unsigned int id,
                            GLenum severity,
                            GLsizei length,
                            const char *message,
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}


static void error_callback(int error, const char* description) {
    std::cerr << "AccelerationStructPreview: " << description << std::endl;
}

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

void AccelerationStructPreview::run(std::string title, int width, int height, std::shared_ptr<Mesh> mesh, std::shared_ptr<Visualizer> visualizer) {



    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    std::string windowTitle = "AccelerationStructPreview [" + title + "]";

    window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);


    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
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
        glBufferData(GL_ARRAY_BUFFER, mesh->getVertices()->size() * sizeof(Vertex), &(mesh->getVertices()->at(0)), GL_STATIC_DRAW);

        glGenBuffers(1, &modelIbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->getIndices()->size() * sizeof(int), &(mesh->getIndices()->at(0)), GL_STATIC_DRAW);

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

    uint32_t wireframeVao;
    uint32_t wireframeVbo;
    uint32_t wireframeIbo;
    uint32_t wireframeVertexShader;
    uint32_t wireframeFragmentShader;
    uint32_t wireframeShaderProgram;
    std::shared_ptr<Mesh> wireframeGeometry = visualizer->generate(*mesh);


    {


        glGenVertexArrays(1, &wireframeVao);
        glBindVertexArray(wireframeVao);

        glGenBuffers(1, &wireframeVbo);
        glBindBuffer(GL_ARRAY_BUFFER, wireframeVbo);
        glBufferData(GL_ARRAY_BUFFER, wireframeGeometry->getVertices()->size() * sizeof(Vertex), &(wireframeGeometry->getVertices()->at(0)), GL_STATIC_DRAW);

        glGenBuffers(1, &wireframeIbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireframeIbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, wireframeGeometry->getIndices()->size() * sizeof(int), &(wireframeGeometry->getIndices()->at(0)), GL_STATIC_DRAW);

        wireframeShaderProgram = glCreateProgram();

        wireframeVertexShader = createShader("Wireframe_Vert", Core::readFile("shaders/preview/Wireframe.vert"), GL_VERTEX_SHADER);
        wireframeFragmentShader = createShader("Wireframe_Frag", Core::readFile("shaders/preview/Wireframe.frag"), GL_FRAGMENT_SHADER);

        glAttachShader(wireframeShaderProgram, wireframeVertexShader);
        glAttachShader(wireframeShaderProgram, wireframeFragmentShader);
        glLinkProgram(wireframeShaderProgram);

        int strideBytes = 7 * sizeof(float);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, strideBytes, static_cast<void*> (nullptr));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideBytes, reinterpret_cast<void*>(4 * sizeof(float)));
        glEnableVertexAttribArray(1);


    }








    auto transform = glm::mat4(1.0f) = glm::scale(glm::mat4(1.0), glm::vec3(0.01f));

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
    float damping = 0.15;


    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


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

            glBindBuffer(GL_ARRAY_BUFFER, modelVbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIbo);

            glDrawElements(GL_TRIANGLES, mesh->getIndices()->size(), GL_UNSIGNED_INT, nullptr);
        }

        //Render the wireframe
        {

            glBindVertexArray(wireframeVao);

            glUseProgram(wireframeShaderProgram);

            int transformLocation = glGetUniformLocation(wireframeShaderProgram, "transform");
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));

            int viewLocation = glGetUniformLocation(wireframeShaderProgram, "view");
            glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

            int projLocation = glGetUniformLocation(wireframeShaderProgram, "projection");
            glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));

            glBindBuffer(GL_ARRAY_BUFFER, wireframeVbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireframeIbo);


            glDrawElements(GL_LINES, wireframeGeometry->getIndices()->size(), GL_UNSIGNED_INT, nullptr);


        }



        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);

    glfwTerminate();



















}