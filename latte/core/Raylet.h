#pragma once
#include <vec4.hpp>
#include <ext/quaternion_common.hpp>

class Raylet {
    glm::vec4 mOrigin;
    glm::vec4 mDirection;
    glm::vec4 mInverseDir;

public:
    Raylet(glm::vec4 origin, glm::vec4 direction) : mOrigin(origin), mDirection(direction) {
        mInverseDir = glm::vec4(1.0) / mDirection;
    }
    glm::vec4 getOrigin() {return mOrigin;}
    glm::vec4 getDirection() {return mDirection;}
    glm::vec4 getInverseDirection() {return mInverseDir;}
};
