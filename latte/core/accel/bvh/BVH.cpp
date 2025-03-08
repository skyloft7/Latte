#include "BVH.h"

#include <algorithm>



void BVH::include(BVHNode& node, glm::vec4 t0, glm::vec4 t1, glm::vec4 t2) {
    node.min = glm::min(node.min, t0);
    node.min = glm::min(node.min, t1);
    node.min = glm::min(node.min, t2);
    node.max = glm::max(node.max, t0);
    node.max = glm::max(node.max, t1);
    node.max = glm::max(node.max, t2);
}

float BVH::cost(BVHNode& node, int numTriangles) {

    glm::vec3 extent = node.max - node.min;
    float area = extent.x * (extent.y + extent.z) + extent.y * extent.z;
    return area * numTriangles;
}

float BVH::evaluateSplit(Mesh& mesh, BVHNode node, int axis, float pos) {
    BVHNode left;
    BVHNode right;

    for (int triangleIndex = node.firstTriangleIndex; triangleIndex < node.firstTriangleIndex + node.triangleCount; triangleIndex++) {
        glm::vec4 t0 = mesh.getVertices()->at(mesh.getIndices()->at(triangleIndex * 3)).position;
        glm::vec4 t1 = mesh.getVertices()->at(mesh.getIndices()->at(triangleIndex * 3 + 1)).position;
        glm::vec4 t2 = mesh.getVertices()->at(mesh.getIndices()->at(triangleIndex * 3 + 2)).position;

        glm::vec4 centroid = (t0 + t1 + t2) / 3.0f;

        if (centroid[axis] < pos) {
            include(left, t0, t1, t2);
            left.triangleCount++;
        }
        else {
            include(right, t0, t1, t2);
            right.triangleCount++;
        }
    }

    return cost(left, left.triangleCount) + cost(right, right.triangleCount);
}

SplitAxisInfo BVH::splitWithSAH(Mesh& mesh, BVHNode node, int numTestsPerAxis) {
    float bestCost = std::numeric_limits<float>::max();
    float bestPos = 0;
    int bestAxis = 0;

    for (int axis = 0; axis < 3; axis++) {
        float min = node.min[axis];
        float max = node.max[axis];

        for (int i = 0; i < numTestsPerAxis; i++) {
            float splitT = (i + 1) / (numTestsPerAxis + 1.0f);
            float pos = min + (max - min) * splitT;
            float cost = evaluateSplit(mesh, node, axis, pos);

            if (cost < bestCost) {
                bestCost = cost;
                bestPos = pos;
                bestAxis = axis;
            }


        }
    }
    return SplitAxisInfo {
        .axis = bestAxis,
        .pos = bestPos,
        .cost = bestCost,
    };

}

void BVH::subdivide(Mesh& mesh, std::vector<BVHNode>& nodes, BVHNode& parent, int maxDepth, int numTestsPerAxis) {



    SplitAxisInfo splitAxisInfo = splitWithSAH(mesh, parent, numTestsPerAxis);
    float parentCost = cost(parent, parent.triangleCount);
    if (splitAxisInfo.cost >= parentCost || parent.depth >= maxDepth) return;

    BVHNode leftNode, rightNode;
    leftNode.firstTriangleIndex = parent.firstTriangleIndex;
    leftNode.depth = parent.depth + 1;
    rightNode.firstTriangleIndex = parent.firstTriangleIndex;
    rightNode.depth = parent.depth + 1;

    for (int triangleIndex = parent.firstTriangleIndex; triangleIndex < parent.firstTriangleIndex + parent.triangleCount; triangleIndex++) {

        glm::vec4 t0 = mesh.getVertices()->at(mesh.getIndices()->at(triangleIndex * 3)).position;
        glm::vec4 t1 = mesh.getVertices()->at(mesh.getIndices()->at(triangleIndex * 3 + 1)).position;
        glm::vec4 t2 = mesh.getVertices()->at(mesh.getIndices()->at(triangleIndex * 3 + 2)).position;
        glm::vec4 centroid = (t0 + t1 + t2) / 3.0f;



        bool isLeft = centroid[splitAxisInfo.axis] < splitAxisInfo.pos;
        BVHNode& childNode = isLeft ? leftNode : rightNode;
        include(childNode, t0, t1, t2);
        childNode.triangleCount++;

        if (isLeft) {
            int swapIndex = childNode.firstTriangleIndex + childNode.triangleCount - 1;


            if (swapIndex * 3 != triangleIndex * 3) {
                std::swap_ranges(
                    mesh.getIndices()->begin() + swapIndex * 3,
                    mesh.getIndices()->begin() + swapIndex * 3 + 3,
                    mesh.getIndices()->begin() + triangleIndex * 3
                );
            }

            rightNode.firstTriangleIndex++;
        }

    }

    nodes.push_back(leftNode);
    parent.leftNodeIndex = nodes.size() - 1;
    nodes.push_back(rightNode);
    parent.rightNodeIndex = nodes.size() - 1;


    subdivide(mesh, nodes, leftNode, maxDepth, numTestsPerAxis);
    subdivide(mesh, nodes, rightNode, maxDepth, numTestsPerAxis);
}


std::shared_ptr<std::vector<BVHNode>> BVH::generate(Mesh& mesh, int maxDepth, int numTestsPerAxis) {
    BVHNode root;

    int totalIndexCount = mesh.getIndices()->size();
    root.firstTriangleIndex = 0;
    root.triangleCount = totalIndexCount / 3;

    //Build the root node
    for (int index = 0; index < root.triangleCount; index++) {
        glm::vec4 t0 = mesh.getVertices()->at(mesh.getIndices()->at(index * 3)).position;
        glm::vec4 t1 = mesh.getVertices()->at(mesh.getIndices()->at(index * 3 + 1)).position;
        glm::vec4 t2 = mesh.getVertices()->at(mesh.getIndices()->at(index * 3 + 2)).position;

        include(root, t0, t1, t2);
    }

    std::vector<BVHNode> nodes;
    nodes.push_back(root);

    subdivide(mesh, nodes, root, maxDepth, numTestsPerAxis);

    return std::make_shared<std::vector<BVHNode>>(nodes);
}
