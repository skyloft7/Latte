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
#include "CPURenderer.h"
#include "Scene.h"
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

    auto scene = std::make_shared<Scene>();


    auto mesh1 = Core::loadMesh("models/capoo.obj");
    {
        auto transform = glm::mat4(1.0f);
        transform = glm::scale(transform, glm::vec3(0.05f));
        transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mesh1->setTransform(transform);

        BVH bvh;
        auto nodes = bvh.generate(*mesh1, 64, 5);
        mesh1->setBVHNodes(nodes);
        mesh1->setMaterial({glm::vec4(1.0, 0.0, 0.0, 1.0)});


        scene->addMesh(mesh1);
    }

    /*
    auto mesh2 = Core::loadMesh("models/capoo.obj");
    {
        auto transform = glm::mat4(1.0f);
        transform = glm::scale(transform, glm::vec3(0.05f));
        transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::translate(transform, glm::vec3(20.0f, 0.0f, 0.0f));

        mesh2->setTransform(transform);

        BVH bvh;
        auto nodes = bvh.generate(*mesh2, 256, 5);
        mesh2->setBVHNodes(nodes);
        mesh2->setMaterial({glm::vec4(0.0, 1.0, 0.0, 1.0)});


        scene->addMesh(mesh2);
    }*/


    std::cout << "Finished generating BVH" << std::endl;



    {
        PerfTimer timer("Main");

        auto renderer = std::make_shared<CPURenderer>();
        renderer->dispatch(scene, {0, 0, (float) width, (float) height}, {0, 0, (float) width, (float) height}, camera);
        renderer->getPixelBuffer()->writeToPNG("single-core-render.png");

    }
    std::cin.get();



}
