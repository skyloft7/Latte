#pragma once
#include <memory>

#include "Camera.h"
#include "Mesh.h"
#include "PixelBuffer.h"
#include "Rect2D.h"
#include "accel/bvh/BVH.h"

class Renderer {
protected:
    std::shared_ptr<PixelBuffer> mPixelBuffer;
    Rect2D mRenderRegion;
public:
    virtual void dispatch(std::shared_ptr<Mesh> mesh, std::shared_ptr<std::vector<BVHNode>> bvhNodes, Rect2D renderRegion, Rect2D totalRegion, Camera camera) = 0;
    std::shared_ptr<PixelBuffer> getPixelBuffer() { return mPixelBuffer; }
};
