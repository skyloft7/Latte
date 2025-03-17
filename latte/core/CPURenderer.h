#pragma once
#include "HitResult.h"
#include "Ray.h"
#include "Renderer.h"
#include "TraceResult.h"

class CPURenderer : public Renderer {
    int depth = 0;
    bool rayTriangleIntersects(const glm::vec3& orig, const glm::vec3& dir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t, glm::vec3& normal);
    float rayIntersectsBVHNode(Ray& raylet, BVHNode& node);
    HitResult rayIntersectsMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Ray ray);
    TraceResult traceRay(Ray& ray, std::shared_ptr<Scene> scene, Camera& camera);


public:
    void dispatch(std::shared_ptr<Scene> scene, Rect2D renderRegion, Rect2D totalRegion, Camera camera) override;


};
