#include <glm.hpp>
#include <iostream>
#include <thread>
#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>

#include "RenderDevice.h"
#include "PixelBuffer.h"
#include "Renderer.h"
#include "../Files.h"
#include "Camera.h"
#include "CPURenderer.h"
#include "Scene.h"
#include "../PerfTimer.h"
#include "Debug.h"

#define screenWidth 100
#define screenHeight 50
#define multithreaded
#define numThreads 2

int main() {

    RenderDevice device = RenderDevice::getCPURenderDevice();

    Camera camera;
    {
        camera.width = screenWidth;
        camera.height = screenHeight;
        glm::vec4 whereToLookAt = glm::vec4(0.0f, 0.0f, 10.0f, 1.0f);
        camera.pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        camera.up = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        camera.right = glm::vec4(glm::cross(glm::vec3(whereToLookAt - camera.pos), glm::vec3(camera.up)), 1.0);
        camera.view = glm::lookAt(glm::vec3(camera.pos),
                 glm::vec3(whereToLookAt),
                 glm::vec3(camera.up));
        camera.fov = glm::radians(45.0f);
        camera.focalLength = 1.0f;
    }

    auto scene = std::make_shared<Scene>();


    auto mesh1 = Files::readMesh("models/capoo.obj");
    {
        auto transform = glm::mat4(1.0f);
        transform = glm::scale(transform, glm::vec3(0.05f));
        transform = glm::translate(transform, glm::vec3(0, -15, 100));
        transform = glm::rotate(transform, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));


        mesh1->setTransform(transform);

        BVH bvh;
        auto nodes = bvh.generate(*mesh1, 256, 5);
        mesh1->setBVHNodes(nodes);
        mesh1->setMaterial({glm::vec3(1.0, 0.0, 0.0), 1.0, 0.2, 0.9});


        scene->addMesh(mesh1);
    }

    auto lightPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    auto lightTransform = glm::mat4(1.0f);
    lightTransform = glm::scale(lightTransform, glm::vec3(0.05f));
    lightTransform = glm::translate(lightTransform, glm::vec3(0, -15, 30.0f));

    auto light = std::make_shared<Light>(lightPos * lightTransform, glm::vec4(1.0, 0.0, 0.0, 1.0), 200.0f);
    scene->addLight(light);


    std::cout << "Finished generating BVH" << std::endl;

#ifdef multithreaded
    {
        PerfTimer timer("Multi-Threaded Render");

        std::vector<std::shared_ptr<std::thread>> threads;
        std::vector<std::shared_ptr<Renderer>> renderers;
        PixelBuffer pixelBuffer(screenWidth, screenHeight);

        for (int i = 0; i < numThreads; i++) {
            auto thread = std::make_shared<std::thread>([&scene, i, camera, &renderers] {
                Rect2D renderRegion = {0, (float) i * (float) screenHeight / (float) numThreads, (float) screenWidth, (float) screenHeight / (float) numThreads};
                auto renderer = std::make_shared<CPURenderer>();
                renderer->dispatch(scene, renderRegion, {0, 0, (float) screenWidth, (float) screenHeight}, camera);
                renderers.push_back(renderer);
            });
            threads.emplace_back(thread);
        }

        for (int i = 0; i < numThreads; i++) {
            auto thread = threads[i];
            thread->join();
            
            auto renderer = renderers[i];
            pixelBuffer.blit(*renderer->getPixelBuffer(), renderer->getRenderRegion());
        }

        pixelBuffer.writeToPNG("multi_threaded_render.png");
    }

#else

    {
        PerfTimer timer("Single-Threaded Render");

        auto renderer = std::make_shared<CPURenderer>();
        renderer->dispatch(scene, {0, 0, (float) width, (float) height}, {0, 0, (float) width, (float) height}, camera);
        renderer->getPixelBuffer()->writeToPNG("single_threaded_render.png");

    }

#endif
    std::cout << "Press any key to exit" << std::endl;
    std::cin.get();



}
