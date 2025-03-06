#include "BVHVisualizer.h"

#include <algorithm>

#include "Visualizer.h"
#include <iostream>
#include <stack>


struct BVHNode {
    glm::vec4 min = glm::vec4(1e30f);
    glm::vec4 max = glm::vec4(-1e30f);
    int firstTriangleIndex = 0;
    int triangleCount = 0;
};
void include(BVHNode& node, glm::vec4 t0, glm::vec4 t1, glm::vec4 t2) {
    node.min = glm::min(node.min, t0);
    node.min = glm::min(node.min, t1);
    node.min = glm::min(node.min, t2);
    node.max = glm::max(node.max, t0);
    node.max = glm::max(node.max, t1);
    node.max = glm::max(node.max, t2);
}

void subdivide(Mesh& mesh, std::vector<BVHNode>& nodes, BVHNode& parent) {

    if (parent.triangleCount < 100) return;

    BVHNode leftNode, rightNode;
    leftNode.firstTriangleIndex = parent.firstTriangleIndex;
    rightNode.firstTriangleIndex = parent.firstTriangleIndex;

    for (int triangleIndex = parent.firstTriangleIndex; triangleIndex < parent.firstTriangleIndex + parent.triangleCount; triangleIndex++) {

        glm::vec4 t0 = mesh.getVertices()->at(mesh.getIndices()->at(triangleIndex * 3)).position;
        glm::vec4 t1 = mesh.getVertices()->at(mesh.getIndices()->at(triangleIndex * 3 + 1)).position;
        glm::vec4 t2 = mesh.getVertices()->at(mesh.getIndices()->at(triangleIndex * 3 + 2)).position;
        glm::vec4 centroid = (t0 + t1 + t2) / 3.0f;

        glm::vec4 extent = parent.max - parent.min;

        int axis = 0;
        if (extent.y > extent.x) axis = 1;
        if (extent.z > extent[axis]) axis = 2;

        bool isLeft = centroid[axis] < parent.min[axis] + extent[axis] / 2;
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
    nodes.push_back(rightNode);

    subdivide(mesh, nodes, leftNode);
    subdivide(mesh, nodes, rightNode);
}

std::shared_ptr<Mesh> BVHVisualizer::generate(Mesh& mesh) {

    std::shared_ptr<std::vector<Vertex>> wireframeVertices = std::make_shared<std::vector<Vertex>>();
    std::shared_ptr<std::vector<int>> wireframeIndices = std::make_shared<std::vector<int>>();
    std::shared_ptr<Mesh> wireframeMesh = std::make_shared<Mesh>("BVH", wireframeVertices, wireframeIndices);

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

    subdivide(mesh, nodes, root);

    for (BVHNode& node : nodes) {
        box(*wireframeMesh, node.min, node.max);
    }


    return wireframeMesh;
}