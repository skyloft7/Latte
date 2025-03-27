#pragma once
#include <glm.hpp>

class Light {
    glm::vec4 mPos;
    glm::vec4 mColor;
    float mIntensity;

public:
    Light(glm::vec4 lightPos, glm::vec4 lightColor, float intensity);
    glm::vec4 getPos();
    glm::vec4 getColor();
    float getIntensity();
};
