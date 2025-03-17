#pragma once
#include <vector>
#include "Mesh.h"


class Scene {
    std::vector<std::shared_ptr<Mesh>> mMeshes;
public:
    void addMesh(std::shared_ptr<Mesh> mesh);
    std::vector<std::shared_ptr<Mesh>> getMeshes();

};
