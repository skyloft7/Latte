#pragma once
#include <vector>

#include "Light.h"
#include "Mesh.h"


class Scene {
    std::vector<std::shared_ptr<Mesh>> mMeshes;
    std::vector<std::shared_ptr<Light>> mLights;

public:
    void addMesh(std::shared_ptr<Mesh> mesh);
    void addLight(std::shared_ptr<Light> light);
    std::vector<std::shared_ptr<Mesh>> getMeshes();
    std::vector<std::shared_ptr<Light>> getLights();

};
