#pragma once
#include <memory>
#include <string>
#include <vector>
#include "glm.hpp"
#include "Vertex.h"

class Mesh {
    std::shared_ptr<std::vector<Vertex>> mVertices;
    std::shared_ptr<std::vector<int>> mIndices;
    std::string mName;

public:
    Mesh(std::string name, std::shared_ptr<std::vector<Vertex>> vertices, std::shared_ptr<std::vector<int>> indices) : mName(name), mVertices(vertices), mIndices(indices) {

    };

    std::shared_ptr<std::vector<Vertex>> getVertices() const {
        return mVertices;
    }

    std::shared_ptr<std::vector<int>> getIndices() const {
        return mIndices;
    }

    std::string getName() const {
        return mName;
    }
};
