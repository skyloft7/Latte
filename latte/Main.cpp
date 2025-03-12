#include "core/Mesh.h"
#include "preview/AccelerationStructPreview.h"
#include "Core.h"
#include "preview/BVHVisualizer.h"
#include "preview/Visualizer.h"



int main() {


	std::shared_ptr<Mesh> mesh = Core::loadMesh("models/capoo.obj");
	std::shared_ptr<Visualizer> visualizer = std::make_shared<BVHVisualizer>();

	AccelerationStructPreview preview;

	preview.run("Stanford Asian Dragon BVH", 960, 540, mesh, visualizer);

	return 0;
}
