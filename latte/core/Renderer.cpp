#include "Renderer.h"

#include <iostream>

#include "Raylet.h"
#include "accel/bvh/BVH.h"



bool Renderer::rayletTriangleIntersects(const glm::vec3& orig, const glm::vec3& dir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t, glm::vec3& N) {
    glm::vec3 v0v1 = v1 - v0;
    glm::vec3 v0v2 = v2 - v0;
    N = glm::cross(v0v1, v0v2);




    float NdotRayDirection = glm::dot(N, dir);

    if (NdotRayDirection > 0.0) return false;

    if (std::fabs(NdotRayDirection) < 1e-8) return false;


    float d = glm::dot(-N, v0);

    t = -(glm::dot(N, orig) + d) / NdotRayDirection;

    if (t < 0) return false;

    glm::vec3 P = orig + t * dir;

    glm::vec3 Ne;

    glm::vec3 v0p = P - v0;
    Ne = glm::cross(v0v1, v0p);
    if (glm::dot(N, Ne) < 0) return false;

    glm::vec3 v2v1 = v2 - v1;
    glm::vec3 v1p = P - v1;
    Ne = glm::cross(v2v1, v1p);
    if (glm::dot(N, Ne) < 0) return false;

    glm::vec3 v2v0 = v0 - v2;
    glm::vec3 v2p = P - v2;
    Ne = glm::cross(v2v0, v2p);
    if (glm::dot(N, Ne) < 0) return false;



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

void Renderer::traceRaylet(glm::vec4& outputColor, std::shared_ptr<Mesh> mesh, BVHNode& bvhNode, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Raylet raylet) {
    if (!rayletBVHNodeIntersects(raylet, bvhNode)) return;

    if (bvhNode.isLeaf) {
        for (int triangleIndex = bvhNode.firstTriangleIndex; triangleIndex < bvhNode.firstTriangleIndex + bvhNode.triangleCount; triangleIndex++) {
            glm::vec4 t0 = mesh->getVertices()->at(mesh->getIndices()->at(triangleIndex * 3)).position;
            glm::vec4 t1 = mesh->getVertices()->at(mesh->getIndices()->at(triangleIndex * 3 + 1)).position;
            glm::vec4 t2 = mesh->getVertices()->at(mesh->getIndices()->at(triangleIndex * 3 + 2)).position;

            glm::vec3 normal;
            float t;

            if (rayletTriangleIntersects(raylet.getOrigin(), raylet.getDirection(), t0, t1, t2, t, normal)) {
                outputColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
                return;
            }

        }
    }
    else {
        traceRaylet(outputColor, mesh, bvhNodes->at(bvhNode.leftNodeIndex), bvhNodes, raylet);
        traceRaylet(outputColor, mesh, bvhNodes->at(bvhNode.rightNodeIndex), bvhNodes, raylet);
    }

}

std::shared_ptr<PixelBuffer> Renderer::dispatchRays(std::shared_ptr<Mesh> mesh, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Rect2D renderRegion, Rect2D totalRegion, Camera camera) {
    auto pixelBuffer = std::make_shared<PixelBuffer>(renderRegion.w, renderRegion.h);


    float aspectRatio = totalRegion.w / (float) totalRegion.h;
    glm::mat4 inverseProj = glm::inverse(camera.proj);


    for (int y = renderRegion.y; y < renderRegion.y + renderRegion.h; y++) {

        int scanlinesRemaining = renderRegion.y + renderRegion.h - y;
        std::cout << "Scanlines Remaining: " << scanlinesRemaining << std::endl;



        for (int x = renderRegion.x; x < renderRegion.x + renderRegion.w; x++) {

            //convert pixel space to ndc

            //give ndc to inverse projection to get back to world space


            glm::vec4 ndc = glm::vec4(x, y, 1, 1) / glm::vec4(totalRegion.w, totalRegion.h, 1, 1) * glm::vec4(2, 2, 1, 1) - glm::vec4(1, 1, 0, 0);
            glm::vec4 rayEnd = inverseProj * glm::vec4(ndc.x, ndc.y, -100.0f, 1.0f);
            Raylet raylet(camera.pos, rayEnd);

            glm::vec4 outputColor(0.0);
            traceRaylet(outputColor, mesh, bvhNodes->at(0), bvhNodes, raylet);
            pixelBuffer->setPixel(x, y, outputColor);


        }
    }




    return pixelBuffer;
}