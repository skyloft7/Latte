#include "BVHVisualizer.h"
#include "Visualizer.h"
#include "../core/accel/bvh/BVH.h"


std::shared_ptr<Mesh> BVHVisualizer::generate(Mesh& mesh) {

    std::shared_ptr<std::vector<Vertex>> wireframeVertices = std::make_shared<std::vector<Vertex>>();
    std::shared_ptr<std::vector<int>> wireframeIndices = std::make_shared<std::vector<int>>();
    std::shared_ptr<Mesh> wireframeMesh = std::make_shared<Mesh>("BVH", wireframeVertices, wireframeIndices);


    BVH bvh;
    auto nodes = bvh.generate(mesh, 8, 5);

    for (const BVHNode& node : *nodes) {
        if (node.depth > 5) box(*wireframeMesh, node.min, node.max);
    }


    return wireframeMesh;
}
