#pragma once
#include "glm.hpp"

struct Camera {
    int width;
    int height;
    glm::vec4 pos = glm::vec4(0.0);
    glm::vec4 up = glm::vec4(0.0);
    glm::vec4 right = glm::vec4(0.0);
    glm::mat4 view = glm::mat4(1.0);
    float fov;
    float focalLength;
};
