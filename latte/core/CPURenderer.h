#pragma once
#include "Ray.h"
#include "Renderer.h"

class CPURenderer : public Renderer {
    bool rayTriangleIntersects(const glm::vec3& orig, const glm::vec3& dir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t, glm::vec3& normal);
    float rayIntersectsBVHNode(Ray& raylet, BVHNode& node);
    bool rayIntersectsMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Ray ray);
public:
    void dispatch(std::shared_ptr<Mesh> mesh, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Rect2D renderRegion, Rect2D totalRegion, Camera camera) override;


};