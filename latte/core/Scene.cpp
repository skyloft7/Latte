#include "Scene.h"

void Scene::addMesh(std::shared_ptr<Mesh> mesh) {
    mMeshes.emplace_back(mesh);
}

std::vector<std::shared_ptr<Mesh>> Scene::getMeshes() {
    return mMeshes;
}
