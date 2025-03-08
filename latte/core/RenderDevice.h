#pragma once
#include <string>

class RenderDevice {
    std::string mName = "";
    uint32_t threadLimit = 0;
    RenderDevice(std::string name, int threadLimit) : mName(name), threadLimit(threadLimit) {}

public:
    static RenderDevice getCPURenderDevice();


};
