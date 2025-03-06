#pragma once
#include "../core/Mesh.h"

class Visualizer {
public:
    virtual std::shared_ptr<Mesh> generate(Mesh& mesh) = 0;

    void box(Mesh& mesh, glm::vec4 min, glm::vec4 max) {
        auto vertices = mesh.getVertices();
        auto indices = mesh.getIndices();

        float minX = min.x, minY = min.y, minZ = min.z;
        float maxX = max.x, maxY = max.y, maxZ = max.z;

        int vertexCount = vertices->size();

        vertices->push_back({ { minX, minY, minZ, 1.0f } });
        vertices->push_back({ { maxX, minY, minZ, 1.0f } });
        vertices->push_back({ { maxX, maxY, minZ, 1.0f } });
        vertices->push_back({ { minX, maxY, minZ, 1.0f } });
        vertices->push_back({ { minX, minY, maxZ, 1.0f } });
        vertices->push_back({ { maxX, minY, maxZ, 1.0f } });
        vertices->push_back({ { maxX, maxY, maxZ, 1.0f } });
        vertices->push_back({ { minX, maxY, maxZ, 1.0f } });


        indices->push_back(vertexCount + 0); indices->push_back(vertexCount + 1);
        indices->push_back(vertexCount + 1); indices->push_back(vertexCount + 2);
        indices->push_back(vertexCount + 2); indices->push_back(vertexCount + 3);
        indices->push_back(vertexCount + 3); indices->push_back(vertexCount + 0);
        indices->push_back(vertexCount + 4); indices->push_back(vertexCount + 5);
        indices->push_back(vertexCount + 5); indices->push_back(vertexCount + 6);
        indices->push_back(vertexCount + 6); indices->push_back(vertexCount + 7);
        indices->push_back(vertexCount + 7); indices->push_back(vertexCount + 4);
        indices->push_back(vertexCount + 0); indices->push_back(vertexCount + 4);
        indices->push_back(vertexCount + 1); indices->push_back(vertexCount + 5);
        indices->push_back(vertexCount + 2); indices->push_back(vertexCount + 6);
        indices->push_back(vertexCount + 3); indices->push_back(vertexCount + 7);

    }


};