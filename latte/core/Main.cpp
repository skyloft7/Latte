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


    std::shared_ptr<Mesh> mesh = Core::loadMesh("models/capoo.obj");


    auto transform = glm::mat4(1.0f);
    transform = glm::scale(transform, glm::vec3(0.05f));
    transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    mesh->setTransform(transform);
    BVH bvh;
    auto nodes = bvh.generate(*mesh, 256, 5);

    std::cout << "Finished generating BVH" << std::endl;


    {
        PerfTimer timer("Main");

        Renderer renderer0;
        renderer0.dispatch(mesh, nodes, {0, 0, (float) width, (float) height}, {0, 0, (float) width, (float) height}, camera);
        renderer0.getPixelBuffer()->writeToPNG("single-core-render.png");


    }
    std::cin.get();

}
