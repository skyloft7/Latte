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
#include "../PerfTimer.h"
#include "../external/vk-bootstrap/VkBootstrap.h"

int main() {


    /*
    vkb::InstanceBuilder instance_builder;
    auto instance_ret = instance_builder.use_default_debug_messenger()
                            .request_validation_layers()
                            .set_headless() // Skip vk-bootstrap trying to create WSI for you
                            .build();
    if (!instance_ret) {
        std::cout << instance_ret.error().message() << "\n";
        return -1;
    }

    vkb::PhysicalDeviceSelector phys_device_selector(instance_ret.value());
    auto phys_device_ret = phys_device_selector.select();
    if (!phys_device_ret) {
        std::cout << phys_device_ret.error().message() << "\n";
        return -1;
    }
    vkb::PhysicalDevice physical_device = phys_device_ret.value();

    vkb::DeviceBuilder device_builder{ physical_device };
    auto device_ret = device_builder.build();
    if (!device_ret) {
        std::cout << device_ret.error().message() << "\n";
        return -1;
    }

    std::cout << device_ret.value().physical_device.name << std::endl;
    */


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

        auto renderer = std::make_shared<CPURenderer>();
        renderer->dispatch(mesh, nodes, {0, 0, (float) width, (float) height}, {0, 0, (float) width, (float) height}, camera);
        renderer->getPixelBuffer()->writeToPNG("single-core-render.png");

    }
    std::cin.get();


}
