#pragma once
#include "Renderer.h"

class GPURenderer : public Renderer {
public:
    void dispatch(std::shared_ptr<Mesh> mesh, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Rect2D renderRegion, Rect2D totalRegion, Camera camera) override;
};