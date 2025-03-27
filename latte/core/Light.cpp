#include "Light.h"

Light::Light(glm::vec4 lightPos, glm::vec4 lightColor, float intensity) : mPos(lightPos), mColor(lightColor), mIntensity(intensity) {

}
glm::vec4 Light::getPos() {
    return mPos;
}
glm::vec4 Light::getColor() {
    return mColor;
}
float Light::getIntensity() {
    return mIntensity;
}