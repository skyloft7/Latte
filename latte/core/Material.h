#pragma once
#include <vec4.hpp>

struct Material {
    glm::vec3 albedo;
    float metallic;
    float roughness;
    float ao;

};
