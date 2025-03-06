#pragma once

#include "Visualizer.h"
#include "../core/Mesh.h"

class AccelerationStructPreview {
public:
    void run(std::string title, int width, int height, std::shared_ptr<Mesh> mesh, std::shared_ptr<Visualizer> visualizer);


};
