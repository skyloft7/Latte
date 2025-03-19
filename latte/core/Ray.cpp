#include "Ray.h"

Ray::Ray(glm::vec4 origin, glm::vec4 direction) : mOrigin(origin), mDirection(direction) {
    mInverseDir = glm::vec4(1.0) / mDirection;
}
glm::vec4 Ray::getOrigin() {return mOrigin;}
glm::vec4 Ray::getDirection() {return mDirection;}
glm::vec4 Ray::getInverseDirection() {return mInverseDir;}