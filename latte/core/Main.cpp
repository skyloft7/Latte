#include <glm.hpp>
#include <iostream>
#include <thread>
#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>

#include "RenderDevice.h"
#include "PixelBuffer.h"
#include "Renderer.h"
#include "../Core.h"
#include "Camera.h"
#include "../PerfTimer.h"

#define MULTITHREADED

int main() {


    int width = 640;
    int height = 480;

    RenderDevice device = RenderDevice::getCPURenderDevice();

    Camera camera;
    {
        camera.width = width;
        camera.height = height;
        glm::vec4 whereToLookAt = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        camera.pos = glm::vec4(0.0f, 0.0f, 3.0f, 1.0f),
        camera.up = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        camera.right = glm::vec4(glm::cross(glm::vec3(whereToLookAt - camera.pos), glm::vec3(camera.up)), 1.0);
        camera.view = glm::lookAt(glm::vec3(camera.pos),
                 glm::vec3(whereToLookAt),
                 glm::vec3(camera.up));
    }


    std::shared_ptr<Mesh> mesh = Core::loadMesh("models/xyzrgb_dragon.obj");
    mesh->setTransform(glm::mat4(0.01));
    BVH bvh;
    auto nodes = bvh.generate(*mesh, 256, 5);

    std::cout << "Finished generating BVH" << std::endl;


    {
        PerfTimer timer("Main");
#ifdef MULTITHREADED

        Renderer renderer0;
        renderer0.dispatchRaysAsync(mesh, nodes, {0, 0, 640, 240}, {0, 0, (float) width, (float) height}, camera);

        Renderer renderer1;
        renderer1.dispatchRaysAsync(mesh, nodes, {0, 240, 640, 240}, {0, 0, (float) width, (float) height}, camera);

        renderer0.wait();
        renderer1.wait();

        PixelBuffer output(width, height);
        output.blit(*renderer0.getPixelBuffer(), {0, 0, 640, 240});
        output.blit(*renderer1.getPixelBuffer(), {0, 240, 640, 240});

        output.writeToPNG("dual-core-render.png");

#else
        Renderer renderer0;
        renderer0.dispatchRaysAsync(mesh, nodes, {0, 0, (float) width, (float) height}, {0, 0, (float) width, (float) height}, camera);
        renderer0.wait();
        renderer0.getPixelBuffer()->writeToPNG("single-core-render.png");
#endif

    }
    std::cin.get();

}
