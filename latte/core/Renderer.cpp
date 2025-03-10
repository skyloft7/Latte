#include "Renderer.h"

#include <iostream>

#include "Raylet.h"
#include "accel/bvh/BVH.h"



bool Renderer::rayletTriangleIntersects(const glm::vec3& orig, const glm::vec3& dir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t, glm::vec3& normal) {

    glm::vec3 v0v1 = v1 - v0;
    glm::vec3 v0v2 = v2 - v0;
    glm::vec3 pvec = glm::cross(dir, v0v2);

    float det = glm::dot(v0v1, pvec);
    if (det < 1e-8) return false;

    normal = glm::cross(v0v1, v0v2);


    float invDet = 1.0f / det;

    glm::vec3 tvec = orig - v0;
    float u = glm::dot(tvec, pvec) * invDet;
    if (u < 0 || u > 1) return false;

    glm::vec3 qvec = glm::cross(tvec, v0v1);
    float v = glm::dot(dir, qvec) * invDet;
    if (v < 0 || u + v > 1) return false;

    t = glm::dot(v0v2, qvec) * invDet;

    return true;
}

bool Renderer::rayletBVHNodeIntersects(Raylet raylet, BVHNode node) {
    glm::vec3 tMin = (node.min - raylet.getOrigin()) * raylet.getInverseDirection();
    glm::vec3 tMax = (node.max - raylet.getOrigin()) * raylet.getInverseDirection();
    glm::vec3 t1 = min(tMin, tMax);
    glm::vec3 t2 = max(tMin, tMax);
    float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
    float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

    bool hit = tFar >= tNear && tFar > 0;
    return hit;
}



void Renderer::traceRaylet(glm::vec4& outputColor, std::shared_ptr<Mesh> mesh, BVHNode& bvhNode, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Raylet raylet, Camera camera) {
    if (!rayletBVHNodeIntersects(raylet, bvhNode)) return;

    if (bvhNode.isLeaf) {
        for (int triangleIndex = bvhNode.firstTriangleIndex; triangleIndex < bvhNode.firstTriangleIndex + bvhNode.triangleCount; triangleIndex++) {
            glm::vec4 t0 = mesh->getTransform() * mesh->getVertices()->at(mesh->getIndices()->at(triangleIndex * 3)).position;
            glm::vec4 t1 = mesh->getTransform() * mesh->getVertices()->at(mesh->getIndices()->at(triangleIndex * 3 + 1)).position;
            glm::vec4 t2 = mesh->getTransform() * mesh->getVertices()->at(mesh->getIndices()->at(triangleIndex * 3 + 2)).position;

            glm::vec3 normal;
            float t;

            if (rayletTriangleIntersects(raylet.getOrigin(), raylet.getDirection(), t0, t1, t2, t, normal)) {
                outputColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
                return;
            }

        }
    }
    else {
        traceRaylet(outputColor, mesh, bvhNodes->at(bvhNode.leftNodeIndex), bvhNodes, raylet, camera);
        traceRaylet(outputColor, mesh, bvhNodes->at(bvhNode.rightNodeIndex), bvhNodes, raylet, camera);
    }

}

void Renderer::wait() {
    mThread.join();
}


void Renderer::dispatchRaysAsync(std::shared_ptr<Mesh> mesh, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Rect2D renderRegion, Rect2D totalRegion, Camera camera) {
    mPixelBuffer = std::make_shared<PixelBuffer>(renderRegion.w, renderRegion.h);
    this->mRenderRegion = renderRegion;

    mThread = std::thread([mesh, bvhNodes, camera, totalRegion, renderRegion, this]() {
        glm::mat4 inverseProj = glm::inverse(camera.proj);

        for (int y = renderRegion.y; y < renderRegion.y + renderRegion.h; y++) {

            int scanlinesRemaining = renderRegion.y + renderRegion.h - y;
            std::cout << "Scanlines Remaining: " << scanlinesRemaining << std::endl;

            for (int x = renderRegion.x; x < renderRegion.x + renderRegion.w; x++) {

                int resX = x - renderRegion.x;
                int resY = y - renderRegion.y;

                glm::vec4 ndc = glm::vec4(x, y, 1, 1) / glm::vec4(totalRegion.w, totalRegion.h, 1, 1) * glm::vec4(2, 2, 1, 1) - glm::vec4(1, 1, 0, 0);
                glm::vec4 rayEnd = inverseProj * glm::vec4(ndc.x, ndc.y, -100.0f, 1.0f);
                rayEnd.y = -rayEnd.y;

                Raylet raylet(camera.pos, rayEnd);

                glm::vec4 outputColor(0.0);
                traceRaylet(outputColor, mesh, bvhNodes->at(0), bvhNodes, raylet, camera);
                this->mPixelBuffer->setPixel(resX, resY, outputColor);


            }
        }

    });

}