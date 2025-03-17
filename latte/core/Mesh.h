#pragma once
#include <memory>
#include <string>
#include <vector>
#include "glm.hpp"
#include "Material.h"
#include "Vertex.h"
#include "BVHNode.h"

class Mesh {
    std::shared_ptr<std::vector<Vertex>> mVertices;
    std::shared_ptr<std::vector<int>> mIndices;
    std::string mName;
    glm::mat4 mTransform = glm::mat4(1.0f);
    std::shared_ptr<std::vector<BVHNode>> mBVHNodes;
    Material mMaterial;

public:
    Mesh(std::string name, std::shared_ptr<std::vector<Vertex>> vertices, std::shared_ptr<std::vector<int>> indices) : mName(name), mVertices(vertices), mIndices(indices) {

    };

    Material getMaterial() {
        return mMaterial;
    }

    void setMaterial(Material material) {
        mMaterial = material;
    }


    void setBVHNodes(std::shared_ptr<std::vector<BVHNode>> bvhNodes) {
        this->mBVHNodes = bvhNodes;
    }

    std::shared_ptr<std::vector<BVHNode>> getBVHNodes() {
        return mBVHNodes;
    }

    std::shared_ptr<std::vector<Vertex>> getVertices() const {
        return mVertices;
    }

    std::shared_ptr<std::vector<int>> getIndices() const {
        return mIndices;
    }

    std::string getName() const {
        return mName;
    }

    void setTransform(const glm::mat4& transform) {
        mTransform = transform;
    }

    glm::mat4 getTransform() const {
        return mTransform;
    }
};

