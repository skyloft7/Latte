#include "Renderer.h"

#include <iostream>

#include "Ray.h"
#include "../PerfTimer.h"
#include "accel/bvh/BVH.h"



bool Renderer::rayTriangleIntersects(const glm::vec3& orig, const glm::vec3& dir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t, glm::vec3& normal) {

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

float Renderer::rayIntersectsBVHNode(Ray& raylet, BVHNode& node) {
    glm::vec3 tMin = (node.min - raylet.getOrigin()) * raylet.getInverseDirection();
    glm::vec3 tMax = (node.max - raylet.getOrigin()) * raylet.getInverseDirection();
    glm::vec3 t1 = glm::min(tMin, tMax);
    glm::vec3 t2 = glm::max(tMin, tMax);
    float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
    float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

    bool hit = tFar >= tNear && tFar > 0;
    float dst = hit ? tNear > 0 ? tNear : 0 : std::numeric_limits<float>::infinity();
    return dst;
}



bool Renderer::rayIntersectsMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Ray ray, int x, int y) {


    int stack[256];
    int stackIndex = 0;

    //Root node
    stack[stackIndex++] = 0;

    while (stackIndex > 0) {
        int nextNodeIndex = stack[--stackIndex];
        BVHNode& node = bvhNodes->operator[](nextNodeIndex);


        if (rayIntersectsBVHNode(ray, node) != std::numeric_limits<float>::infinity()) {
            if (node.isLeaf) {


                for (int triangleIndex = node.firstTriangleIndex; triangleIndex < node.firstTriangleIndex + node.triangleCount; triangleIndex++) {
                    glm::vec4 t0 = mesh->getTransform() * mesh->getVertices()->operator[](mesh->getIndices()->operator[](triangleIndex * 3)).position;
                    glm::vec4 t1 = mesh->getTransform() * mesh->getVertices()->operator[](mesh->getIndices()->operator[](triangleIndex * 3 + 1)).position;
                    glm::vec4 t2 = mesh->getTransform() * mesh->getVertices()->operator[](mesh->getIndices()->operator[](triangleIndex * 3 + 2)).position;

                    glm::vec3 normal;
                    float t;

                    if (rayTriangleIntersects(ray.getOrigin(), ray.getDirection(), t0, t1, t2, t, normal)) {
                        return true;
                    }
                }

            }
            else {
                BVHNode& leftNode = bvhNodes->operator[](node.leftNodeIndex);
                BVHNode& rightNode = bvhNodes->operator[](node.rightNodeIndex);

                float dstLeft = rayIntersectsBVHNode(ray, leftNode);
                float dstRight = rayIntersectsBVHNode(ray, rightNode);

                if (dstLeft > dstRight) {
                    stack[stackIndex++] = node.leftNodeIndex;
                    stack[stackIndex++] = node.rightNodeIndex;
                }
                else {
                    stack[stackIndex++] = node.rightNodeIndex;
                    stack[stackIndex++] = node.leftNodeIndex;
                }
            }
        }

    }

    return false;
}

void Renderer::wait() {
    mThread.join();
}


void Renderer::dispatchRaysAsync(std::shared_ptr<Mesh> mesh, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Rect2D renderRegion, Rect2D totalRegion, Camera camera) {
    mPixelBuffer = std::make_shared<PixelBuffer>(renderRegion.w, renderRegion.h);
    this->mRenderRegion = renderRegion;



    mThread = std::thread([mesh, bvhNodes, camera, totalRegion, renderRegion, this]() {



        for (int y = renderRegion.y; y < renderRegion.y + renderRegion.h; y++) {

            int scanlinesRemaining = renderRegion.y + renderRegion.h - y;

            std::cout << "Scanlines Remaining: " << scanlinesRemaining << std::endl;

            for (int x = renderRegion.x; x < renderRegion.x + renderRegion.w; x++) {



                int resX = x - renderRegion.x;
                int resY = y - renderRegion.y;



                glm::vec4 ndc = glm::vec4(x, y, 1, 1) / glm::vec4(totalRegion.w, totalRegion.h, 1, 1) * glm::vec4(2, 2, 1, 1) - glm::vec4(1, 1, 0, 0);
                glm::vec4 rayEnd = glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f);
                rayEnd.y = -rayEnd.y;

                Ray ray(camera.pos, rayEnd);

                //glm::vec4 outputColor((float) resX / (float) renderRegion.w, (float) resY / (float) renderRegion.h, 0.0f, 1.0f);

                glm::vec4 outputColor(0.0);



                //if (std::numeric_limits<float>::infinity() != rayIntersectsBVHNode(ray, bvhNodes->at(0))) outputColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
                if (rayIntersectsMesh(mesh, bvhNodes, ray, x, y)) outputColor = glm::vec4(1.0, 0.0, 0.0, 1.0);


                this->mPixelBuffer->setPixel(resX, resY, outputColor);

            }
        }



    });

}