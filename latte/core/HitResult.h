#pragma once
#include <vec4.hpp>

struct HitResult {
    bool miss = true;
    float t;
    glm::vec4 normal;
};
