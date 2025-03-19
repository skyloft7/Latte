#pragma once
#include <vec4.hpp>

struct Material {
    glm::vec4 albedo;
    float emission;
    float reflectivity;
};
