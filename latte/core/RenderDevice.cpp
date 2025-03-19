#include "RenderDevice.h"

#include <thread>

RenderDevice::RenderDevice(std::string name, int threadLimit) : mName(name), mThreadLimit(threadLimit){

}


RenderDevice RenderDevice::getCPURenderDevice() {
    return {"CPU0", static_cast<int>(std::thread::hardware_concurrency())};
}
