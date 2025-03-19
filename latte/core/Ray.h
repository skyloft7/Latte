#pragma once
#include <vec4.hpp>
#include <ext/quaternion_common.hpp>

class Ray {
    glm::vec4 mOrigin;
    glm::vec4 mDirection;
    glm::vec4 mInverseDir;

public:
    Ray(glm::vec4 origin, glm::vec4 direction);
    glm::vec4 getOrigin();
    glm::vec4 getDirection();
    glm::vec4 getInverseDirection();
};
