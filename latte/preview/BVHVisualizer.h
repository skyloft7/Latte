#pragma once

#include "Visualizer.h"

class BVHVisualizer : public Visualizer {
    std::shared_ptr<Mesh> generate(Mesh& mesh) override;
};