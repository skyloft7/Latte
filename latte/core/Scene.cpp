#include "Scene.h"

void Scene::addMesh(std::shared_ptr<Mesh> mesh) {
    mMeshes.emplace_back(mesh);
}

void Scene::addLight(std::shared_ptr<Light> light) {
    mLights.emplace_back(light);
}

std::vector<std::shared_ptr<Mesh>> Scene::getMeshes() {
    return mMeshes;
}
std::vector<std::shared_ptr<Light>> Scene::getLights() {
    return mLights;
}


