#pragma once

struct TraceResult {
    bool miss = true;
    glm::vec4 color;
    glm::vec4 normal;
    glm::vec4 hit;
    std::shared_ptr<Mesh> mesh;
};