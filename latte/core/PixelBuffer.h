#pragma once
#include <vector>
#include <string>
#include <vec4.hpp>

class PixelBuffer {
    std::vector<glm::vec4> mVector;
    int mWidth, mHeight;


public:
    PixelBuffer(int width, int height) : mWidth(width), mHeight(height) {}
    void setPixel(int x, int y, glm::vec4 color);
    glm::vec4 getPixel(int x, int y);
    void exportImage(std::string path);
    int getWidth() { return mWidth; }
    int getHeight() { return mHeight; }


};
