#include <vec4.hpp>

struct BVHNode {
    glm::vec4 min = glm::vec4(1e30f);
    glm::vec4 max = glm::vec4(-1e30f);
    int firstTriangleIndex = 0;
    int triangleCount = 0;
    int depth = 0;
    int leftNodeIndex = 0;
    int rightNodeIndex = 0;
    bool isLeaf = true;
};
