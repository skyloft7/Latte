#include "AccelerationStructPreview.h"
#include <iostream>



#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Core.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


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

void AccelerationStructPreview::run(std::string title, int width, int height, std::shared_ptr<Mesh> mesh) {



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



    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    uint32_t vbo;
    {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh->getVertices()->size() * sizeof(glm::vec4), &(mesh->getVertices()->at(0)), GL_STATIC_DRAW);
    }



    uint32_t ibo;
    {
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->getIndices()->size() * sizeof(int), &(mesh->getIndices()->at(0)), GL_STATIC_DRAW);
    }






    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    {

        const char* vertexShaderSource;

        try {
            vertexShaderSource = Core::readFile("shaders/AccelerationStructPreview.vert").c_str();
        }
        catch (std::invalid_argument& e) {
            std::cout << e.what() << std::endl;
        }

        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);



        glCompileShader(vertexShader);


        int vertexShaderCompileSuccess;
        char vertexShaderCompileInfoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderCompileSuccess);

        if(!vertexShaderCompileSuccess) {
            glGetShaderInfoLog(vertexShader, 512, NULL, vertexShaderCompileInfoLog);
            std::cout << "Failed to compile vertex shader: \n" << vertexShaderCompileInfoLog << std::endl;
        }

    }

    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    {

        const char* fragmentShaderSource;
        try {
            fragmentShaderSource = Core::readFile("shaders/AccelerationStructPreview.frag").c_str();
        }
        catch (std::invalid_argument& e) {
            std::cout << e.what() << std::endl;
        }

        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);

        int fragmentShaderCompileSuccess;
        char fragmentShaderCompileInfoLog[512];
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompileSuccess);

        if(!fragmentShaderCompileSuccess) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, fragmentShaderCompileInfoLog);
            std::cout << "Failed to compile fragment shader: \n" << fragmentShaderCompileInfoLog << std::endl;
        }
    }

    uint32_t shaderProgram = glCreateProgram();
    {
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
    }

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*> (nullptr));
    glEnableVertexAttribArray(0);

    auto transform = glm::mat4(1.0f) = glm::scale(glm::mat4(1.0), glm::vec3(0.01f));

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);


    glUseProgram(shaderProgram);

    int transformLocation = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));

    int viewLocation = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

    int projLocation = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));













    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        glBindVertexArray(vao);
        glUseProgram(shaderProgram);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        glDrawElements(GL_TRIANGLES, mesh->getIndices()->size(), GL_UNSIGNED_INT, nullptr);




        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);

    glfwTerminate();



















}