#include "GPURenderer.h"
#include <iostream>
#include "../Core.h"



void GPURenderer::dispatch(std::shared_ptr<Mesh> mesh, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Rect2D renderRegion, Rect2D totalRegion, Camera camera) {
    mPixelBuffer = std::make_shared<PixelBuffer>(renderRegion.w, renderRegion.h);
    this->mRenderRegion = renderRegion;

    std::cout << "GPURenderer::dispatch" << std::endl;




}

