#include <glm.hpp>
#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>

#include "RenderDevice.h"
#include "PixelBuffer.h"
#include "Renderer.h"
#include "../Core.h"
#include "Camera.h"

int main() {

    RenderDevice device = RenderDevice::getCPURenderDevice();

    Camera camera;
    {
        camera.width = 426;
        camera.height = 240;
        glm::vec4 whereToLookAt = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        camera.pos = glm::vec4(0.0f, 0.0f, 3.0f, 1.0f),
        camera.up = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        camera.right = glm::vec4(glm::cross(glm::vec3(whereToLookAt - camera.pos), glm::vec3(camera.up)), 1.0);
        camera.proj = glm::perspective(glm::radians(45.0f), (float)camera.width/(float)camera.height, 0.1f, 100.0f);
        camera.view = glm::lookAt(glm::vec3(camera.pos),
                 glm::vec3(whereToLookAt),
                 glm::vec3(camera.up));
    }


    std::shared_ptr<Mesh> mesh = Core::loadMesh("models/xyzrgb_dragon.obj");
    mesh->setTransform(glm::mat4(0.01));
    Renderer renderer;
    BVH bvh;
    auto nodes = bvh.generate(*mesh, 128, 5);
    std::shared_ptr<PixelBuffer> output = renderer.dispatchRays(mesh, nodes, {0, 0, 426, 240}, {0, 0, 426, 240}, camera);


    output->exportImage("output.png");
}
