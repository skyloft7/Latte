#include "RenderDevice.h"

#include <thread>

RenderDevice RenderDevice::getCPURenderDevice() {
    return {"CPU0", static_cast<int>(std::thread::hardware_concurrency())};
}
