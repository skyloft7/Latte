#pragma once
#include "../../Mesh.h"
#include "BVH.h"

struct BVHNode;

struct SplitAxisInfo {
    int axis = 0;
    float pos = 0;
    float cost = 0;
};

class BVH {
    std::vector<BVHNode> nodes;
    void include(BVHNode& node, glm::vec4 t0, glm::vec4 t1, glm::vec4 t2);
    float cost(BVHNode& node, int numTriangles);
    float evaluateSplit(Mesh& mesh, BVHNode node, int axis, float pos);
    SplitAxisInfo splitWithSAH(Mesh& mesh, BVHNode node, int numTestsPerAxis);
    void subdivide(Mesh& mesh, std::vector<BVHNode>& nodes, BVHNode& parent, int maxDepth, int numTestsPerAxis);

public:
    virtual std::shared_ptr<std::vector<BVHNode>> generate(Mesh& mesh, int maxDepth, int numTestsPerAxis);
};