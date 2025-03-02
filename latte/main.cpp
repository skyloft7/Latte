#include <deque>
#include <iostream>

#include "core/Mesh.h"
#include "preview/AccelerationStructPreview.h"
#include "Core.h"


int main() {


	std::cout << "Latte!" << std::endl;

	std::shared_ptr<Mesh> mesh = Core::loadMesh("models/xyzrgb_dragon.obj");


	AccelerationStructPreview preview;
	preview.run("Preview Demo", 640, 480, mesh);



	return 0;
}
