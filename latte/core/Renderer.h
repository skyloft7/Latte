#pragma once
#include <thread>

#include "Camera.h"
#include "Mesh.h"
#include "PixelBuffer.h"
#include "Ray.h"
#include "Rect2D.h"
#include "accel/bvh/BVH.h"

class Renderer {
    std::thread mThread;
    std::shared_ptr<PixelBuffer> mPixelBuffer;
    Rect2D mRenderRegion;
    bool rayTriangleIntersects(
            const glm::vec3& orig, const glm::vec3& dir,
            const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
            float& t, glm::vec3& normal);
    float rayIntersectsBVHNode(Ray& raylet, BVHNode& node);
    bool rayIntersectsMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Ray raylet, int x, int y);
public:
    void dispatchRaysAsync(std::shared_ptr<Mesh> mesh, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Rect2D renderRegion, Rect2D totalRegion, Camera camera);
    void wait();
    std::shared_ptr<PixelBuffer> getPixelBuffer() { return mPixelBuffer; }
    Rect2D getRenderRegion() { return mRenderRegion; }
};
