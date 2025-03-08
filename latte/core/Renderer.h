#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "PixelBuffer.h"
#include "Raylet.h"
#include "Rect2D.h"
#include "accel/bvh/BVH.h"

class Renderer {
    bool rayletTriangleIntersects(
            const glm::vec3& orig, const glm::vec3& dir,
            const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
            float& t, glm::vec3& N);
    bool rayletBVHNodeIntersects(Raylet raylet, BVHNode node);
    void traceRaylet(glm::vec4& outputColor, std::shared_ptr<Mesh> mesh, BVHNode& bvhNode, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Raylet raylet);
public:
    std::shared_ptr<PixelBuffer> dispatchRays(std::shared_ptr<Mesh> mesh, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Rect2D renderRegion, Rect2D totalRegion, Camera camera);

};
