#pragma once
#include <string>

class RenderDevice {
    std::string mName = "";
    uint32_t mThreadLimit = 0;
    RenderDevice(std::string name, int threadLimit);

public:
    static RenderDevice getCPURenderDevice();


};
