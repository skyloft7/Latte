#include "Mesh.h"

Material Mesh::getMaterial() {
    return mMaterial;
}

void Mesh::setMaterial(Material material) {
    mMaterial = material;
}


void Mesh::setBVHNodes(std::shared_ptr<std::vector<BVHNode>> bvhNodes) {
    this->mBVHNodes = bvhNodes;
}

std::shared_ptr<std::vector<BVHNode>> Mesh::getBVHNodes() {
    return mBVHNodes;
}

std::shared_ptr<std::vector<Vertex>> Mesh::getVertices() {
    return mVertices;
}

std::shared_ptr<std::vector<int>> Mesh::getIndices() {
    return mIndices;
}

std::string Mesh::getName() {
    return mName;
}

void Mesh::setTransform(const glm::mat4& transform) {
    mTransform = transform;
}

glm::mat4 Mesh::getTransform() {
    return mTransform;
}